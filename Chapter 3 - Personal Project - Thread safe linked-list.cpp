#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

//In designing thread safe, one must choose between 2 evils. Deadlock or Race condition. Deadlock happens when too many locks, and race condition happens when there's too little lock covered

template<typename T>
class LinkedList {
	//Implementing multiple locks
	struct Node {
		mutex m;
		T data{ 0 };
		Node* nextptr{ nullptr };
		Node* prevptr{ nullptr };
	};
	Node* head{ nullptr };
public:
	LinkedList () = default;
	LinkedList (const LinkedList& rhs) noexcept {
		scoped_lock guard{ rhs.head->m };	//Protect the current looking head
		if (!rhs.head)
			return;
		Node* cur_ptr = head;
		Node* rhs_ptr = rhs.head;

	}
};

int main () {
}