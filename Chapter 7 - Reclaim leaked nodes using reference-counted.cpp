//#include <iostream>
//#include <atomic>
//#include <memory>
//using namespace std;
//
//template <typename T>
//class lockfree_stack {
//private:
//    struct node {
//        shared_ptr<T> data;
//        node* next;
//        node (T const& data_) : data (make_shared<T> (data_)) {}
//    };
//    atomic<node*> head;
//
//    //! This whole section below is for deleteing and cleaning up leaked nodes
//
//    atomic<node*> to_be_deleted;   //the list of nodes to delete
//    static void delete_nodes (node* nodes) {  //static because there's no reason for having
//                                            //multiple instance of this
//        while (nodes) {     //Delete everything, no lock
//            node* next = nodes->next;
//            delete nodes;
//            nodes = next;
//        }
//    }
//
//    void try_reclaim (node* oldHead) {
//        if (threads_in_pop == 1) {  //Only 1 thread popping ==> Maybe Safe to considering deleting all leaking nodes
//            nodes* nodes_to_delete = to_be_deleted.exchange (nullptr);  //Take over the leaking nodes list to delete
//            if (!(--threads_in_pop)) {  //Have to check again in case a thread pop another node into the delete list
//                delete_nodes (nodes_to_delete);
//            }
//            //Something has been change between the check and the take over
//            else if (nodes_to_delete) {
//                chain_pending_nodes (nodes_to_delete);  //Then put the delete nodes back into the leaking nodes list
//            }
//            delete old_head;    //Since there's only 1 threads popping, we can
//            //*Reason: pop() always return the last node. If there are multiple pop(), there could be a chance
//                //*we pop() the same nodes--> It's not safe to delete our ONE leaking node yet. But if there's only one pop(),
//                //*then we can safely delete our own leaking node
//        }
//        else {  //Multiple thread popping ==> Put the leaking node to the leaking list
//            chain_pending_node (old_head);
//            --threads_in_pop;
//        }
//    }
//
//    /**
//     *Chain the list of nodes to the front of the leaking nodes list
//     *@param nodes The begining of the list of nodes
//    */
//    void chain_pending_nodes (node* nodes) {
//        node* last = nodes;
//        while (node* const next = last->next) { //Go to the end of the list
//            last = next;
//        }
//        chain_pending_nodes (nodes, last);  //List of nodes to chain = [ range from nodes - the last nodes ]
//    }
//
//    /**
//     *Chain the list of nodes with range to the leaking nodes list, set the head to the first node in the range afterwards
//     */
//    void chain_pending_nodes (node* first, node* last) {
//        last->next = to_be_deleted; //Chain the end of the list to the existing leaking list
//        while (!to_be_deleted.compare_exchange_weak (last->next, first));   //After chaining put the "head" to first node of the
//                                                                         //new leaked list
//            //*The "to_be_deleted" is an atomic variable and can be changed at anytime by the thread, thus we need to make
//        //*sure we chain it to the list correctly.
//    }
//
//    void chain_pending_node (node* n) {
//        chain_pending_nodes (n, n); //There's only 1 node need to be chain
//    }
//public:
//    shared_ptr<T>pop () {
//        ++threads_in_pop;
//        node* oldHead = head.load ();
//        while (oldHead && !head.compare_exchange_weak (oldHead, oldHead->next));
//        shared_ptr<T> res;
//        if (oldHead) {
//            res.swap (oldHead->data);   //Swap data out because when we pop we gonna throw away the nodes anyway
//                                    //==> No need for the data inside the node that is leaked
//        }
//        try_reclaim (oldHead);  //Start the "trying" delete leaked nodes list
//        return res;
//    }
//};