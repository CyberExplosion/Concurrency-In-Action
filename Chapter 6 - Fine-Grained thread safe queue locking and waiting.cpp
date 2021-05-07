#include <exception>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

template <typename T>
class threadsafe_queue {
private:
	struct node {
		shared_ptr<T> data;	//Easy transfer, no need to construct a T when put val in
		unique_ptr<node> next;	//No need for delete command
	};
	mutex head_mutex;	//Need a mutex for each data ==> Fine grained
	mutex tail_mutex;
	unique_ptr<node> head;
	node* tail;	//Since we do going to treat tail=head as empty, we need tail to be normal 
				//pointer so we can access the node if its pointed to by both head and tail
	condition_variable cond;

	//Helper functions
	node* get_tail ();	//Only need to acquire tail to get it, no need for anything after this function
	unique_ptr<node> pop_head ();	//Safety get the old_head, no lock inside
	unique_lock<mutex> wait_for_data ();	//Just for waiting if there's data on the queue
	unique_ptr<node> wait_pop_head ();	//Safety get the head, uses the above func to wait
	unique_ptr<node> wait_pop_head (T& value);	//Safety get the head, also passing the value

	unique_ptr<node> try_pop_head ();	//just like above, but no wait version
	unique_ptr<node> try_pop_head (T& value);
public:
	threadsafe_queue () :head (new node), tail (head.get ()) {};
	threadsafe_queue (const threadsafe_queue&) = delete;	//Can't copy unique pointer
	threadsafe_queue& operator= (const threadsafe_queue&) = delete; //No safe assignment
			//with mutex and unique_pointer
	shared_ptr<T> try_pop ();	//Will see if can get a data out or not
	bool try_pop (T& value);	//Same, 2nd method
	shared_ptr<T> wait_and_pop ();	//Keep trying until can pop
	void wait_and_pop (T& value);	//2nd method
	void push (T neo_value);
	bool empty ();
};



//Definition

template <typename T>
threadsafe_queue<T>::node* threadsafe_queue<T>::get_tail () {
	scoped_lock guard{ tail_mutex };
	return tail;
}

template <typename T>	//Need to tell the compiler that the threadsafe_queue<T>::node is a type
						//and not a template name
unique_ptr<typename threadsafe_queue<T>::node> threadsafe_queue<T>::pop_head () {	//No lock
	unique_ptr<node> old_head = std::move (head);
	head = std::move (old_head->next);	//Move the head backward
	return old_head;
}

template <typename T>
//This function is here to make sure that the data is locked and wait properly b4 continue
unique_lock<mutex> threadsafe_queue<T>::wait_for_data () {
	unique_lock guard{ head_mutex };
	cond.wait (guard, [&] () {return head.get () != get_tail (); });
	return guard;	//The outside will need to get the lock using a move
}

template<typename T>
unique_ptr<typename threadsafe_queue<T>::node> threadsafe_queue<T>::wait_pop_head () {
	unique_lock guard{ wait_for_data () };	//Since it's rvalue it's gonna be auto move
	return pop_head ();	//Got the lock so it's safe
}

template<typename T>
unique_ptr<typename threadsafe_queue<T>::node> threadsafe_queue<T>::wait_pop_head (T& value) {
	unique_lock guard{ wait_for_data () };
	value = std::move (*head->data);	//Addition to copy the value out, also return the ptr
	return pop_head ();
}

template<typename T>
unique_ptr<typename threadsafe_queue<T>::node> threadsafe_queue<T>::try_pop_head () {
	scoped_lock guard{ head_mutex };
	if (head.get () == get_tail ()) {	//Only need to get tail, don't even need lock for	
									//comparison because the order is ez to define
			//The tail either occurs b4 push --> old tail
			//Tail occurs after push --> latest tail. Both case doesn't change matter much
		return unique_ptr<node> ();	//empty
	}
	return pop_head ();
}

template<typename T>
unique_ptr<typename threadsafe_queue<T>::node> threadsafe_queue<T>::try_pop_head (T& value) {
	scoped_lock guard{ head_mutex };
	if (head.get () == get_tail ()) {	//Only need to get tail, don't even need lock for	
									//comparison because the order is ez to define
			//The tail either occurs b4 push --> old tail
			//Tail occurs after push --> latest tail. Both case doesn't change matter much
		return unique_ptr<node> ();	//empty
	}
	value = std::move (*head->data);
	return pop_head ();
}



int main () {

}

//Public
template<typename T>
shared_ptr<T> threadsafe_queue<T>::try_pop () {
	unique_ptr<node> old_head = try_pop_head ();	//Can throw here if there's not enough mem
	return old_head ? old_head->data : shared_ptr<T> ();	//If old head is empty return empty
}

template<typename T>
bool threadsafe_queue<T>::try_pop (T& value) {
	auto const old_head = try_pop_head (value);	//We only care the value and the 
			//boolean anyway, might as well convey the const for people to know
	return old_head ? true : false;
}

template<typename T>
shared_ptr<T> threadsafe_queue<T>::wait_and_pop () {
	auto const old_head = wait_pop_head ();	//Only care about the inside of head, not head itself
	return old_head->data;
}

template<typename T>
void threadsafe_queue<T>::wait_and_pop (T& value) {
	auto const old_head = wait_pop_head (value);
}

template<typename T>
void threadsafe_queue<T>::push (T neo_value) {
	shared_ptr<T> neo_data = make_shared<T> (std::move(neo_value));
	unique_ptr<node> dum (new node);
	{	//Remember: currently, tail is at a dummy node
		scoped_lock guard{ tail_mutex };
		tail->data = std::move (neo_data);
		auto neo_tail = dum.get ();
		tail->next = std::move (dum);	//Have to use this roundabout way so
								//we don't have to access a moved object
		tail = neo_tail;
	}
	cond.notify_one ();	//Notify others there's work to do
}

template<typename T>
bool threadsafe_queue<T>::empty () {
	scoped_lock guard{ head_mutex };
	return head.get () == get_tail ();
}
