#include <atomic>
#include <memory>
using namespace std;

//! External count (increase) = before accessing data in the node
//! Internal count (decrease) = after finish working in the node

//? We need both an inside and outside counter
/**
 *? Inside counter to check and make sure that we can delete the nodes while already insdie
 *? Outside counter to make sure that we can access the nodes without fear of accessing an empty pointer
 *? Internal nodes will check with external node to make sure that we won't delete node if someone trying to access
 */
 template <typename T=int>
 class lockfree_stack {
 private:
     struct node;
     struct count_node_ptr {     //Helper nodes to determine how many threads referencing the ptr
         int external_count;
         node* ptr;
     };
     struct node {
         shared_ptr<T> data;
         atomic<int> internal_count;
         count_node_ptr next;
         node (T const& data_) : data (make_shared<T> (data_)), internal_count (0) {}
     };
     atomic<count_node_ptr> head;    //There's no need for any data in the head

     void increase_head_count (count_node_ptr& old_counter) {    //Use this function when you can access a node
         count_node_ptr neoCounter;
         do {
             neoCounter = old_counter;
             ++neoCounter.external_count;
         } while (!head.compare_exchange_strong (old_counter, neoCounter));  //Since we in a stack, this is to make
             //sure that no other thread has changed the head --> Of course use this after you set old_counter = head
         old_counter.external_count = neoCounter.external_count;    //You are both accessing the head and the old_counter, so increment both of them in a sense
     }
 public:
     lockfree_stack () = default;
     lockfree_stack (const lockfree_stack&) = delete;
     lockfree_stack operator=(const lockfree_stack&) = delete;

     bool quickCheck () {
         const atomic<count_node_ptr>* ptr = &head;
         return atomic_is_lock_free (ptr);
     }
     ~lockfree_stack () {
         while (pop ());
     }

     void push (T const& data) {
         count_node_ptr neoNode;
         neoNode.ptr = new node (data);
         neoNode.external_count = 1; //One referencing the node right now (the head pointer, when the head got move then it's going to be the previous node in the list)
         neoNode.ptr->next = head.load ();
         while (!head.compare_exchage_weak (neoNode.ptr->next, neoNode));    //Head points right here
     }

     shared_ptr<T> pop () {
         count_node_ptr old_head = head.load ();
         while (true) {
             increase_head_count (old_head); //The head is pointing at the node ==> +1
             node* const ptr = old_head.ptr; //Access the real ptr inside
             if (!ptr) { //nullptr
                 return shared_ptr<T> ();
             }
             //If pass, other threads can no longer access this node from stack
             if (head.compare_exchange_strong (old_head, ptr->next)) {   //Head no longer point at node => -1
                 shared_ptr<T> res;
                 res.swap (ptr->data);   //This thread don't need to access this node anymore also => -1
                 int const count_increase = old_head.external_count - 2;  //SO we minus 2 at the end
                 if (ptr->internal_count.fetch_add (count_increase) == -count_increase) {    //* fetch_add() returns old value so
                             //* if the count_increase equal the negative of count_incrase (which if you sum will be 0), then no other thread
                             //* can access nor see this node
                     delete ptr;
                 }
                 return res;
             }
             //Fail means some thread has push or pop the node we looking at already
             //If push then we have to restart from the begining and pop a different node
             //The below condition checked for if our current node is POPPED already (not pushed)
             else if (ptr->internal_count.fetch_sub (1) == 1) {  //* If the count is 1, means that we are the only thread left that seeing this node
                 delete ptr;
             }
         }
     }
 //     //* Notice we can't touch the internal count until we get a hold of the node. Sum of external and internal counts is total reference
 //     //* external count only increase, internal count only decrease
 //     //* External count increase when a thread trying to read the node, Internal count decrease when finish
 //     //* Once the node is taken over (no other thread can see it on stack). The external count is minused 1 (-1) because head no longer point at it
 //     //* Then we sum the external to the internal and use internal as the TOTAL REFERENCE COUNT
 // };

struct node;
struct count_node_ptr {     //Helper nodes to determine how many threads referencing the ptr
    int external_count;
    node* ptr;
};

struct node {
    shared_ptr<int> data;
    atomic<int> internal_count;
    count_node_ptr next;
    node (int const& data_) : data (make_shared<int> (data_)), internal_count (0) {}
};



int main () {
    atomic<count_node_ptr> yeh;
    if (yeh.is_lock_free ())
        printf ("E");
}