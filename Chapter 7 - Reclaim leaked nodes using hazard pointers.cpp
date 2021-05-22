#include <atomic>
#include <memory>
#include <thread>
#include <functional>
using namespace std;

template <typename T>
class lockfree_stack {
private:
    struct node {
        shared_ptr<T> data;
        node* next;
        node (T const& data_) : data (make_shared<T> (data_)) {}
    };
    atomic<node*> head;

    shared_ptr<T>pop () {
        atomic<void*>& hp = get_hazard_pointer_for_current_thread ();
        node* old_head = head.load ();
        do {
            node* temp;
            do {
                temp = old_head;    //Use temp to make sure that head didn't change (no other thread pop)
                hp.store (old_head);   //Loop until you got the hazard pointer equal head
                old_head = head.load ();
            } while (old_head != temp);
            //Using strong because if we fail spuriously then we have to do another atomic operation
        } while (old_head && !head.compare_exchange_strong (old_head, old_head->next));
        hp.store (nullptr); //Clear hazard pointer, you already claimed the old_head
        shared_ptr<T> res;
        if (old_head) {
            res.swap (old_head->data);  //Get data out cause this node will be leaked
            if (outstanding_hazard_pointers_for (old_head)) {   //Check if other hazard pointer referencing the node you claimed
                reclaim_later (old_head);   //There are other thread reference it, can't delete yet
            }
            else {
                delete old_head;
            }
        }
        delete_nodes_with_no_hazards ();    //Try delete nodes that was leaked
    }
    return res;
};

unsigned const max_hazard_pointers = 100;
//! Multiple thread could be going through the hazard_pointer_arr so we need to make each member atomic
struct hazard_pointer {
    atomic<std::thread::id> id; //This represents the id of the thread requesting hazard pointer
    atomic<void*> pointer;  //Make void* so that it can takes any kind of pointers
        //*This will be the pointer that will point to leaked nodes
};

hazard_pointer hazard_pointer_arr[max_hazard_pointers]; //This is global

class hp_owner {
    hazard_pointer* hp;
public:
    hp_owner (hp_owner const&) = delete;
    hp_owner operator= (hp_owner const&) = delete;

    hp_owner () : hp (nullptr) {
        for (size_t i = 0; i < max_hazard_pointers; i++) {
            thread::id old_id;  //Construct a default id
            /**
             ** It will loop through the list and check for a free slot (default constructed id) to the ID
             ** of the current theard (thread that asking for a slot). Then that slot will become the hazard pointer
            */
            if (hazard_pointer_arr[i].id.compare_exchange_strong (old_id, this_thread::get_id ())) {
                hp = &hazard_pointer_arr[i];
                break;
            }
        }
        if (!hp) {  //couldn't find a free slot
            throw runtime_error ("No hazard pointers available");
        }
    }
    atomic<void*>& get_pointer () {
        return hp->pointer;
    }

    ~hp_owner () {  //Before destroy the owner object, make sure to turn the slot in the array back into default state
        hp->pointer.store (nullptr);
        hp->id.store (thread::id ());
    }
};

atomic<void*>& get_hazard_pointer_for_current_thread () {
    thread_local static hp_owner hazard;    //Each thread will get their own hazard pointer, no access between threads
    //Also make it static so we don't reinitialize the hazard pointer everytime (costly).
    //? Besides, shouldn't the hazard pointer be the same everytime we call this function. If we don't make it
        //? static, the hazard object will be destroy and recreate when we call it later for no reason
    return hazard.get_pointer ();
}

//* Scan thorugh the table and find if there's an hazard pointer to node p
bool outstanding_hazard_pointers_for (void* p) {
    for (size_t i = 0; i < max_hazard_pointers; ++i) {
        if (hazard_pointer_arr[i].pointer.load () == p) {
            return true;
        }
    }
    return false;
}

//! All below are helper function for reclaim_later and delete_nodes_with_no_hazard()
template <typename T>
void do_delete (void* p) {
    delete static_cast<T*>(p);  //When delete actual data, you have to cast from void*
}

struct data_to_reclaim {    //A list of nodes
    void* data;
    std::function<void (void*)> deleter;    //Create a function object
    data_to_reclaim* next;

    template <typename T>
    data_to_reclaim (T* p) : data (p), deleter (&do_delete<T>), next (0) {}    //Reference because why copy

    ~data_to_reclaim () {
        deleter (data);
    }
};
atomic<data_to_reclaim*> nodes_to_reclaim;

void add_to_reclaim_list (data_to_reclaim* node) {
    node->next = nodes_to_reclaim.load ();
    while (!nodes_to_reclaim.compare_exchange_weak (node->next, node)); //Check before moving the head, just like in push
}


template <typename T>   //*We make it a template because then we can use it on type other than just 'node'
void reclaim_later (T* data) {
    add_to_reclaim_list (new data_to_reclaim (data));
}

//? Bad thing about this is that for every pop, we would have to scan the entire leaked nodes list
//? You can make it better by only scan it when the number of leaked nodes reach (2 * max_leaked_nodes)
//? By that point, it's guarantee that if you free the nodes, there will be at least max_leaked_nodes been free
//? Think: if you wait (max_hazard *2) - max_hazard = You will reclaim at least max_hazard nodes
/***
 *It's because you already have max_leaked_node array, that means it won't be able to pop more leaked
 nodes until you free some of the slots. If you wait till (max_leaked_node + 1) until you start reclaiming
, then you will guarantee to have 1 free slot.
 */
void delete_nodes_with_no_hazards () {
    data_to_reclaim* current = nodes_to_reclaim.exchange (nullptr); //Transfer owner to this thread
    while (current) {
        data_to_reclaim* const next = current->next;
        if (!outstanding_hazard_pointers_for (current->data)) { //data is the node itself
            delete current;
        }
        else {
            add_to_reclaim_list (current); //There's still hazard pointer. Move it back to the list
        }
        current = next;
    }
}
//! But that means we have to waste another atomic to create a count for each thread to wait until the leaked list size is (max_hazard * 2)