//#include <iostream>
//#include <mutex>
//#include <thread>
//using namespace std;
//
////Fine grain locking
////In designing thread safe, one must choose between 2 evils. Deadlock or Race condition. Deadlock happens when too many locks, and race condition happens when there's too little lock covered
//
//template<typename T>
//class LinkedList {
//	//Implementing multiple locks
//	struct Node {
//		mutex m;
//		T data{ 0 };
//		Node* nextptr{ nullptr };
//		Node* prevptr{ nullptr };
//	};
//	Node* head{ nullptr };
//public:
//	LinkedList () = default;
//	LinkedList (const LinkedList& rhs) noexcept {
//		//scoped_lock guard{ rhs.head->m, head->m };	//Protect the current looking head
//		if (!rhs.head)
//			return;
//		try {
//			head = new Node ();
//			lock (rhs.head->m, head->m);	//Lock mutex when first created
//			head->data = rhs.head->data;
//
//			Node* cur_ptr = head->nextptr;
//			Node* prev_ptr = head;
//			Node* rhs_ptr = rhs.head->nextptr;
//			rhs.head->m.unlock ();
//			head->m.unlock ();
//
//			while (!rhs_ptr->nextptr) {
//				cur_ptr = new Node ();
//				//Following one lock direction
//				if (cur_ptr->prevptr && rhs_ptr->prevptr) {	//Check if the previous node exist before attempting to lock
//					scoped_lock guard{ cur_ptr->prevptr->m, rhs_ptr->prevptr->m };
//				}
//
//				cur_ptr->data = rhs_ptr->data;
//				cur_ptr->prevptr = prev_ptr;
//				prev_ptr = cur_ptr;		//Set previous node
//				cur_ptr = cur_ptr->nextptr;		//Move to next node
//			}
//		} catch (const bad_alloc& e) {
//			cout << e.what ();
//			throw(e);
//		}
//	}
//};
//
//int main () {
//	LinkedList<int> l1;
//	LinkedList<int> l2 = l1;
//}