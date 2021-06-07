#include <thread>
#include <stack>
#include <mutex>
#include <exception>
using namespace std;

struct empty_stack : std::exception {
	const char* what () const noexcept;
};

template <typename T>
class threadsafe_stack {
private:
	stack<T> data;
	mutable mutex m;
public:
	threadsafe_stack () {};
	threadsafe_stack (const threadsafe_stack& other) {
		scoped_lock guard{ m };
		data = other.data;	//Use the stack assignment op
	}
	threadsafe_stack& operator= (const threadsafe_stack&) = delete;	//Can't use assignment
	//no assignment op for mutex. Also only copy-swap idiom makes assignment exception safe
	void push (T new_value) {	//Copy in because thread
		scoped_lock guard{ m };
		data.push (move (new_value));	//Might throw an exception if the move fail
									//Stack guarantee push to be exception free tho so it's good
	}
	shared_ptr<T> pop () {	//1 option to return pointer. (Can't return reference because they 
							//can change from the outside
		scoped_lock guard{ m };
		if (data.empty ()) throw empty_stack ();
		shared_ptr<T> const res{ make_shared<T> (move (data.top ())) };	//Move so no need to 
						//copy the file into making a new value. Also avoid making new data
		//Also const so you can't do anything funny like swap the pointer or reset it
		data.pop ();
		return res;
	}
	void pop (T& value) {	//2 option to pass in a reference for result
		scoped_lock guard{ m };
		if (data.empty ()) throw empty_stack ();
		value = move(data.top ());	//Assign to reference so no exception
						//The move or the inner copy assignment can throw
		data.pop ();
	}
	bool empty () const {
		scoped_lock guard{ m };
		return data.empty ();	//Empty never throw so this function is safe
	}
};