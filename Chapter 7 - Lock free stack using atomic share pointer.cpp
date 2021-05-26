//#include <memory>
//#include <atomic>
//#include <iostream>
//using namespace std;
//
//int main () {
//    const shared_ptr<int>* check;
//    if (atomic_is_lock_free (check)) {
//        cout << "Do use this to ease of mind\n";
//    }
//    else {
//        cout << "Use manually reference counting\n";
//    }
//}
//
////! ONLY USE THIS IF THE ABOVE CHECK IS PASSED, SHARED_PTR COULD BE USING A LOCK TO SIMULATE ATOMIC
//template <typename T>
//class lockfree_stack {
//private:
//    struct node {
//        shared_ptr<T> data;
//        shared_ptr<node> next;
//        node(T const& data_): data(make_shared<T> (data_)){}
//    };
//    shared_ptr<node> head;  //We going to use the atomic function to access atomically
//public:
//    void push (T const& data) {
//        shared_ptr<node> const neoNode = make_shared<T> (data);
//        neoNode->next = std::atomic_load (&head);
//        while (!atomic_compare_exchange_weak (&head, &neoNode->next, &neoNode));
//    }
//    shared_ptr<T> pop () {
//        shared_ptr<node> oldHead = atomic_load (head);
//            //The oldHead->next node could have been access by another thread
//        while (oldHead && atomic_compare_exchange_weak (&head, &oldHead, atomic_load (oldHead->next))); //Load because multiple thread could be popping the same node
//        if (old_head) {
//            atomic_store (&old_head->next, shared_ptr<node> ());    //Remove the popped node reference quickly to hasten the reference counting in the pointed node
//            return old_head->data;
//        }
//    }
//    ~lockfree_stack () {
//        while (pop ()); //Until lass node is nullptr
//    }
//};