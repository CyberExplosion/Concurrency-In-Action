//#include <mutex>
//using namespace std;
//
//template <typename T>
//class threadsafe_list {
//private:
//	struct node {
//		mutex m;
//		shared_ptr<T> data;
//		unique_ptr<node> next;
//		node () : next () {}
//		node (T const& value) :data (std::make_shared<T> (value)) {}
//	};
//	node head;
//public:
//	threadsafe_list () {}
//	~threadsafe_list () {
//		remove_if ([] (node const&) {return true; });	//This use the remove_if below
//	}
//	threadsafe_list (threadsafe_list const&) = delete;
//	threadsafe_list& operator= (threadsafe_list const&) = delete;
//
//	void push_front (T const& value) {
//		unique_ptr<node> neonode{ new node (value) };
//		scoped_lock guard{ head.m };
//		neonode->next = std::move (head.next);
//		head.next = std::move (neonode);
//	}
//
//	/*Self exercise*/
//	//insert the new node before the value in the pos
//	bool insert (T const& pos, T const& val) {
//		node* current = &head;
//		unique_ptr<node>neoNode{ new node (val) };
//		unique_lock lk = head.m;
//		while (node* const nextNode = current->next.get ()) {
//			unique_lock nxt_lk = nextNode->m;
//			if (*nextNode.data == pos) {
//				neoNode->next = std::move (nextNode);
//				current.next = std::move (neoNode);
//				return true;
//			} else {
//				lk.unlock ();
//				current = nextNode;
//				lk = std::move (nxt_lk);
//			}
//		}
//		return false;
//	}
//
//	template <typename Function>
//	void for_each (Function f) {	//Can't use iterator efficiently in concurrency ==> make it as close as possible
//							//we usually use iterator for these std function anyway, may as well provide them 
//							//so we don't need to make iterator
//		node* current = &head;
//		unique_lock cur_lk{ head.m };
//		while (node* const nextNode = current->next.get ()) {	//Checking if the next node exist
//			unique_lock nxt_lk (nextNode->m);	//Lock the next node
//			cur_lk.unlock ();	//No need for the cur lock anymore, already got a hold of the next one
//			f ((*nextNode)->data);
//			current = nextNode;
//			cur_lk = std::move (nxt_lk);	//Transition lock when things are done
//		}
//	}
//
//	template <typename Predicate>
//	shared_ptr<T> find_first_if (Predicate p) {	//Predicate is a function return true or false
//		node* current = &head;
//		unique_lock cur_lk{ head.m };
//		while (node* const nextNode = current->next.get ()) {
//			unique_lock nxt_lk{ nextNode->m };
//			cur_lk.unlock ();
//			if (p ((*nextNode)->data)) {
//				return nextNode->data;
//			}
//			current = nextNode;
//			cur_lk = std::move (nxt_lk);
//		}
//	}
//
//	template <typename Predicate>
//	void remove_if (Predicate p) {
//		node* current = &head;
//		unique_lock cur_lk{ head.m };
//		while (node* const nextNode = current->next.get ()) {
//			unique_lock nxt_lk{ nextNode->m };
//			if (p(*nextNode->data)) {
//				unique_ptr<node> trash = std::move (current->next);
//				current->next = std::move (nextNode->next);
//				nxt_lk.unlock ();	//Can't destroy the node with the lock on
//					//No worry, the cur_lock is not unlock yet, so there's no way any thread reaching this node
//				//When the trash pointer go out of scope, it will destroy the node
//			} else {
//				cur_lk.unlock ();
//				current = nextNode;
//				cur_lk = std::move (nxt_lk);
//			}
//		}
//	}
//
//
//};
//
//int main () {
//	threadsafe_list<int> test;
//
//}
//
///*
//Most function involving moving pointers around is non-throw, so we are good
//*/