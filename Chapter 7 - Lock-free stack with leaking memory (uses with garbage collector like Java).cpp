#include <memory>
#include <atomic>
using namespace std;

template <typename T>
class lockfree_stack {
private:
	struct node {
		shared_ptr<T> data;
		node* next;
		node (T const& data_) : data(make_shared<T>(data_)){}
	};
	atomic<node*> head;
public:
	/*
	1. Create a new node
	2. Set its next pointer to the current head
	3. Set the head pointer to the new node
	*/
	void push (T const& data) {
		node* const neoNode = new node (data);
		neoNode->next = head.load ();
		while (!head.compare_exchange_weak (neoNode->next, neoNode));	//Only move forward if
				//the head reads is the same head that reads b4. This essentially only continue
				//when there's no other thread move the head ==> Guarantee push the node on top
	}

	/*
	1. Read the current value of head
	2. Read head->next
	3. Set head to head->next
	4. Return the data retrieved from node
	5. Delete the retrieved node (not able to yet)
	*/
	shared_ptr<T> pop () {
		node* oldHead = head.load ();
		//First check if the old head can be dereference == not end of stack
		while (oldHead && !head.compare_exchange_weak (oldhead, oldHead->next));	//Check if 
			//we removing the *top* of the stack (goat be the head still that we remove)
			//If only then, we can move the head
		//Essentially this only pop the node if it's the only thread popping -> No race
		return oldHead ? oldHead->data : make_shared<T> ();
	}
};

//Data after being remove is still not deallocate yet --> Need something to delete the nodes later
int main () {

}