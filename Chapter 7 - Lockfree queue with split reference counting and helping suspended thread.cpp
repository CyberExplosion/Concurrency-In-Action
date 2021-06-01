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
        atomic<counted_node_ptr> next;  //! Make atomic to allow multiple thread to help push if the operation is blocking other threads
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

    //* declare static when you think it's "general" and doesn't tie to any member variable or anything
    static void increase_external_count (atomic<counted_node_ptr>& counter, counted_node_ptr& old_counter);
    static void free_external_counter (counted_node_ptr& old_node_ptr);

    //! Extern function make just to update the tail, WE also determine inside if the tail update is by us, or by a helper thread
    void set_new_tail (counted_node_ptr& old_tail, counted_node_ptr const& new_tail) {
        node* const current_tail_ptr = old_tail.ptr;    //Get the real ptr out
        while (!tail.compare_exchange_weak (old_tail, new_tail) &&  //*Use weak because we also account for spurious, beside no operation inside loop
                //?  old_tail will be assigned with the A NEW TAIL if other thread help move the tail
                //If another thread is helping you with the tail, you don't want to release_ext_counters(tail) or else it would be bad
                //* If the compare is true, it means we are the one setting the tail
            old_tail.ptr == current_tail_ptr);  //Use this to check that the tail is actually helped by other thread, (the compare could be just fail spuriously)

        if (old_tail.ptr = current_tail_ptr) {  //We are the one change the tail
            free_external_counter (old_tail);
        }
        else {  //Other thread helped with the tail
            current_tail_ptr->release_ref ();   //No job left for us, release everything
        }
    }
public:

    void push (T new_value) {
        unique_ptr<T> new_data (new T (new_value));
        counted_node_ptr new_next;
        new_next.ptr = new node ();
        new_next.external_count = 1;
        counted_node_ptr old_tail = tail.load ();
        while (true) {
            increase_external_count (tail, old_tail);
            T* old_data = nullptr;
            if (old_tail.ptr->data.compare_exchange_strong (old_data, new_data.get ())) {   //Check if the tail is our
                counted_node_ptr old_next = {0};
                if (!old_tail.ptr->next.compare_exchange_strong (old_next, new_next)) { //The old_tail->next node (aka dummy node) is already been dealt with
                    //* Also, old_next is assigned the pointer the other thread helped make after this condition
                    delete new_next.ptr;    //We no longer need the node we create at begining
                    new_next = old_next;    //Now we get the pointer the other thread helped created
                }
                set_new_tail (old_tail, new_next);  //Tail is assigned the new_next, there's a chance other thread already helped moving the tail
                new_data.release ();
                break;
            }
            else {  //Some other thread get tail => Help them
                counted_node_ptr old_next = {0};
                if (old_tail.ptr->next.compare_exchange_strong (old_next, new_next)) { //Help the suspended thread deal with the dummy node (ptr->next)
                    old_next = new_next;    //We going to use old_next as our tail
                    new_next.ptr = new node (); //Make a node at that location,
                            //The suspended thread will delete their node once they found out we helped them
                }
                set_new_tail (old_tail, old_next);
            }
        }
    }

    unique_ptr<T> pop () {
        counted_node_ptr old_head = head.load (memory_order_relaxed);
        while (true) {
            increase_external_count (head, old_head);   //Increase count and also reassign old_head = head
            node* const ptr = old_head.ptr;
            if (ptr == tail.load ().ptr) {
                ptr->release_ref ();
                return unique_ptr<T> ();
            }
            counted_node_ptr next = ptr->next.load ();
            if (head.compare_exchange_strong (old_head, next)) {
                T* const res = ptr->data.exchange (nullptr);
                free_external_counter (old_head);
                return unique_ptr<T> (res);
            }
            ptr->release_ref ();
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
        //* At this point the tail = old_tail again
    /**
     * (counter = tail, old_counter = old_tail)
     * The loop condition set tail = neo_counter, then afterward old_tail = neo_counter ==> tail = old_tail
     */

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