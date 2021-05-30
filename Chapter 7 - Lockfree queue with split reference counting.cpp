#include <atomic>
#include <memory>
using namespace std;

template <typename T>
class lockfree_queue {
private:
    struct node;
    struct counted_node_ptr {
        int external_count;
        node* ptr;
    };
    atomic<counted_node_ptr> head;  //This will lead to we have 2 counters to count the external access
    atomic<counted_node_ptr> tail;
    struct node_counter {   //* If we make the size of the struct as close to 32-bit : a machine word (integer size usually)
                            //* Most machine would more likely to treat atomic operation on this lock-free
        unsigned internal_count : 30;
        unsigned external_counters : 2; //Bit field specification operation, we only need 2 bits for the counters.
                //* The maximum of counters one node can have pointing to is either both the head and tail at same node
                //* or the tail and the node before it points to the new node (when insert new node) ==> max is 2
    };
    struct node {
        atomic<T*> data;    //Happen because push()
        atomic<node_counter> count; //Multiple thread can edit the count
        counted_node_ptr next;
        node () {
            node_counter neo_count;
            neo_count.internal_count = 0;
            neo_count.external_counters = 2;    //Tail = 1 counters, the previous node = 1 counters. (Each counter then can count other thread trying to get access using external_count)
            count.store (neo_count);

            next.ptr = nullptr; //We'll use nullptr to check that if we are the only thread pushing in new node
            next.internal_count = 0;    //No one accessing node yet
        }

        void release_ref ();
    };

    //Both the function below doesn't do anything to special to each instance of the class (aka doesn't use this pointer), so can be made static
    /**
    *Without static:
    *   Math math = new Math()
    *   double y = math.sin(x)
    *With static:
    *   double y = Math.sin(x)
    *
    */
    //* declare static when you think it's "general" and doesn't tie to any member variable or anything
    static void increase_external_count (atomic<counted_node_ptr>& counter, counted_node_ptr& old_counter);
    static void free_external_counter (counted_node_ptr& old_node_ptr);

public:
    //* Since we treat tail as a dummy node that doesn't have any value inside, its' a problem when multiple thread tries to push()
    //* To create a happens-b4 relation ship (push then pop),  the tail can only be store() after everything has been done (setting up dummy nodes data)
    //* But to set up dummy nodes data, you first need to load() the tail in. SO, After the load() and before the store(), thread will racing to access the data inside the tail
    //? WHY CAN"T WE USE TAIL IN COMPARE EXCHANGE  JUST LIKE  WE NORMALLY DO WITH HEAD
    //! We can't use the compare_exchange with tail because tail is different from head
    //* Normal we would do tail.compare_exchange(old_tail, old_tail->next) sth like that. You can't do old_tail->next = tail.load() because that only works for head (old_head->next = head.load())
    //* If you do tail.compare_exchange(old_tail, old_tail->next), and it success. Then tail would be assign old_tail->next, however, that data doesn't exist because we haven't created it
    //* If you created old_tail->next after you do the old_tail = tail.load(), then u violate the law, YOU CHANGE THE TAIL DATA WITHOUT MAKING SURE YOU"RE THE ONLY THREAD has access to it
    //? You need a way to make sure still that you're the only thread has access to the tail somehow
    //* If you make the data itself atomic, you can compare using the data instead of the tail. If the data is not exist in the tail, it means that you're the only one. If there is, then some thread got there b4 you
    void push (T neo_val) {
        unique_ptr<T> neo_data (new T (neo_val));
        counted_node_ptr neo_next;
        neo_next.ptr = new node (); //setting up dummy node for tail
        neo_next.external_count = 1;    //We are about to access into this new node
        counted_node_ptr old_tail = tail.load ();

        while (true) {
            increase_external_count (tail, old_tail);   //We about to access the tail node, increment the external count
            T* old_data = nullptr;
            if (old_tail.ptr->data.compare_exchange_strong (old_data, neo_data.get ())) {   //Only go through if the data is still nullptr (aka no thread has change the tail)
                //Now has acquire the tail successfully
                old_tail.ptr->next = neo_next;
                old_tail = tail.exchange (neo_next);    //? think you can remove the old_tail and just use tail.exchange() alone and it would still works
                free_external_counter (old_tail);   //The tail counter no longer need this node
                neo_data.release ();    //The data will now own by the node, not the unique_ptr
                break;
            }
            old_tail.ptr->release_ref ();   //We done everything with the tail, release all references (counts, all external)
        }
    }

    unique_ptr<T> pop () {
        counted_node_ptr old_head = head.load (memory_order_relaxed);
        while (true) {
            increase_external_count (head, old_head);   //About to access inner data
            node* const ptr = old_head.ptr; //Get the actual node out
            if (ptr == tail.load ().ptr) {  //head = tail ===> Empty queue
                ptr->release_ref ();    //Remove all counts to the node
                return unique_ptr<T> ();
            }
            if (head.compare_exchange_strong (old_head, ptr->next)) {
                T* const res = ptr->data.exchange (nullptr);   //Remove the data from the node after retrieve it
                free_external_counter (old_head);   //The head counter no longer need this node
                return unique_ptr<T> (res);
            }
            ptr->release_ref ();    //Done with everything on this node
        }
    }
};

template <typename T>
void lockfree_queue<T>::node::release_ref () {
    node_counter old_counter = count.load (memory_order_relaxed);
    node_counter neo_counter;
    do {
        neo_counter = old_counter;
        --neo_counter.internal_count;
    } while (!count.compare_exchange_strong (old_counter, neo_counter, memory_order_acquire, memory_order_relaxed));
        //* strong because if fail spuriously, have to go through 2 operation again, wasteful
        //* also the memory order is there to follow closely as the memory order uses in lockfree stack method
    if (!neo_counter.internal_count && !neo_counter.external_counters) {    //When both are 0, nothing reference the node anymore
        delete this;    //this here is the node, not the queue
    }
}

//Since the function below doesn't access any member variable and only works its' arguments, it should be declare static
template <typename T>
static void lockfree_queue<T>::increase_external_count (atomic<counted_node_ptr>& counter, counted_node_ptr& old_counter) {
    counted_node_ptr neo_counter;
    do {
        neo_counter = old_counter;
        ++neo_counter.external_count;
    } while (!counter.compare_exchange_strong (old_counter, neo_counter, memory_order_acquire, memory_order_relaxed));
    old_counter.external_count = neo_counter.external_count;    //Update both the counter, and the old_counter
}

template <typename T>
static void lockfree_queue<T>::free_external_counter (counted_node_ptr& old_node_ptr) {
    node* const ptr = old_node_ptr.ptr;
    int const count_increase = old_node_ptr.external_count - 2; //Remove from the head (or tail), and from the previous node in the list
    node_counter old_counter = ptr->count.load (memory_order_relaxed);
    node_counter neo_counter;
    do {    //Since the count is atomic, if we do anything inside the count, it has to been in a loop like this
        neo_counter = old_counter;
        --neo_counter.external_counters;    //Use this in pop or push, so one less head or tail counter will exist
        neo_counter.internal_count += count_increase;
    } while (!ptr->count.compare_exchange_strong (old_counter, neo_counter, memory_order_acquire, memory_order_relaxed));
    if (!neo_counter.internal_count && !neo_counter.external_counters) {
        delete ptr;
    }
}