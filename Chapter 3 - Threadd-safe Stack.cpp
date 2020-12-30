//#include <iostream>
//#include <thread>
//#include <mutex>
//#include <stack>
//using namespace std;
//
////Lock the stack with each operations so that no more than 1 thread can change the stack itself
//
//struct EmptyStack : std::exception {
//	const char* what () const noexcept {
//		cout << "Stack is empty\n";
//	}
//};
//
//template <typename T>
//class ThreadSafe_Stack {
//private:
//	stack<T> data;
//	mutex m;
//public:
//	ThreadSafe_Stack () = default;
//	ThreadSafe_Stack (const ThreadSafe_Stack& rhs) {
//		scoped_lock guard{ rhs.m };		
//		data = rhs.data;	//Data copy inside rather than using initialization list, because we need to lock
//	}
//	ThreadSafe_Stack& operator=(const ThreadSafe_Stack& rhs) = delete;	//Disallow assignment operator because lock cannot be assign
//	void push (T val) {
//		scoped_lock guard{ m };
//		data.push (val);
//	}
//
//	//Using option 3, returning a pointer to the new object == Cannot cause race condition and allow flexibility
//	shared_ptr<T> pop () {
//		scoped_lock guard{ m };
//		if (data.empty ())
//			throw EmptyStack ();
//		shared_ptr<T> re_val (make_shared<T> (data.top ()));	//Create a pointer for the value first
//		data.pop ();	//Then pop it, just to be safe
//		return re_val;
//	}
//	//Since the pointers only allow outside to interact with the "member" of the stack (not the stack itself), we didn't violate the lock protection
//
//	//Using option 1, return through parameters, backward but ensure no exception can happen
//	void pop (T& re_val) {
//		scoped_lock guard{ m };
//		if (data.empty ())
//			throw EmptyStack ();
//		re_val = data.top ();
//		data.pop ();
//	}		//Return by value (copy) allow the potential for the caller to get an exception if they don't have enough
//			//memory for the copied out value. Thus if we return through the reference, we ensure no exception
//
//	bool empty () const {
//		scoped_lock guard{ m };
//		return data.empty ();
//	}
//};