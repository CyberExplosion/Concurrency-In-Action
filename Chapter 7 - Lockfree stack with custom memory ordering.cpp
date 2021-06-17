#include <atomic>
#include <memory>
using namespace std;

template <typename T>
class lockfree_stack {
private:
    struct node;
    struct counted_node_ptr {
        int external_count;
        node* ptr;
    };
    struct node {
        shared_ptr<T> data;
        atomic<int> internal_count; //Multiple thread could changing this -->atomic
        counted_node_ptr next;
        node (T const& data_) : data (make_shared<T> (data_)), internal_count (0) {};
    };
    atomic<counted_node_ptr> head;  //Same reason, multiple thread could move the head
    void increase_head_count (counted_node_ptr& old_counter) {
        counted_node_ptr neo_counter;
        do {
            neo_counter = old_counter;
            ++neo_counter;
        } while (!head.compare_exchange_strong (old_counter, neo_counter, memory_order_acquire, memory_order_relaxed));
        //* Acquire will synchronize with the push() to make sure that the ptr is visibly valid on this thread
        //* Since this is a read-modify-write operation, this compare_exchange is in the release sequence
            //* That means other RMW operation can safely acquire without the fear of stealing
            //*the release from other threads, only when a load (memory_order_acquire) will take the release flag 4ever
        old_counter.external_count = neo_counter.external_count;
    }

public:
    ~lockfree_stack () {
        while (pop ());
    }

    void push (T const& data) {
        counted_node_ptr neo_node;
        neo_node.ptr = new node (data);
        neo_node.external_count = 1;
        neo_node.ptr->next = head.load (memory_order_relaxed);
        while (!head.compare_exchange_weak (neo_node.ptr->next, neo_node, memory_order_release, memory_order_relaxed));
        //* release so that the neo_node.ptr is valid for thread tries to access the data inside
    }

    shared_ptr<T> pop () {
        counted_node_ptr old_head = head.load (memory_order_relaxed);
        while (true) {
            increase_head_count (old_head); //! Push() will be synchronized-with here, now the ptr inside the node should be valid
            node* const ptr = old_head;
            if (!ptr) {
                return shared_ptr<T> ();
            }
            //! Notice you don't need to synchronize with the push() here to have safety in accessing the data inside ptr
            //! It's because the increase_head_count() already perform the synchronize-with
            //! and it's before this line, so we have happens-before relationship
            if (head.compare_exchange_strong (old_head, ptr->next, memory_order_relaxed)) {
                shared_ptr<T> res;
                res.swap (ptr->data);   //Dangerous here if ptr is not seen as valid from push()
                int const count_increase = old_head.external_count - 2;
                //! It's dangerous to delete ptr without knowing if another thread is still accessing or not
                //! We need to make sure that swap happens before any thread tries to delete the ptr

                /**
                 ** If the thread failed to acquire the head=old_head, it KNOW that other thread has taken over that node
                 ** However, in the failed branch, we tried to delete the node. That is not good as the other thread that
                 ** has the node is doing .swap(). Therefore, we need to make sure there's syncs between those threads
                 ** to only delete after swap
                 */
                //Use internal_count as a device to provide synchronize-with
                if (ptr->internal_count.fetch_add (count_increase, memory_order_release) == -count_increase) {
                    delete ptr; //Delete is a dangerous operation as it modify data that are using by other threads
                }
                return res;
            }
            //! fetch_add() is a RMW operation so it will automatically synchronize with the release sequence, even with relaxed ordering
            //! Just need to get rid of the release sequence by using the load() inside
            else if (ptr->internal_count.fetch_add (-1, memory_order_relaxed) == 1) {
                ptr->internal_count.load (memory_order_acquire);    //Synchronize-here, now this thread will see that the swap happen before the ptr being deleted
                delete ptr;
            }
        }
    }
};

int main(){
    printf ("Hello");
}