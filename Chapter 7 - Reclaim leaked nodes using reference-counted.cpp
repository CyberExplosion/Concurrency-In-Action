#include <iostream>
#include <atomic>
#include <memory>
using namespace std;

template <typename T>
class lockfree_stack {
private:
    struct node {
        shared_ptr<T> data;
        node* next;
        node (T const& data_) : data (make_shared<T> (data_)) {}
    };
    atomic<node*> to_be_deleted;   //the list of nodes to delete
    static void delete_nodes (node* nodes) {  //static becaues there's no reason for having
                                            //multiple instance of this
        while (nodes) {     //Delete everything, no lock
            node* next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }

    void try_reclaim (node* oldHead) {
        if (threads_in_pop == 1) {
            nodes* nodes_to_delete = to_be_deleted.exchange (nullptr);
        }
    }

public:
    int what;

};