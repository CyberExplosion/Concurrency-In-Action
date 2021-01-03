//#include <queue>
//#include <iostream>
//#include <mutex>
//#include <condition_variable>
//using namespace std;
//
////Apply method 1 and 3 to prevent exception when combining pop() and top(). Return through pointers and by reference.
////Since reference cannot throw an exception and neither are by reference
//
//template <typename T>
//class threadsafe_queue {
//private:
//	mutable mutex m;
//	queue<T> data_queue;
//	condition_variable data_cond;
//
//public:
//	threadsafe_queue () = default;
//	threadsafe_queue (threadsafe_queue const& rhs) {
//		scoped_lock guard{ m };
//		data_queue = rhs.data_queue;
//	}
//	void push (T val) {
//		{
//			scoped_lock guard{ m };
//			data_queue.push (val);
//		}
//		data_cond.notify_one ();
//	}
//	void wait_and_pop (T& val) {	//Add a wait and pop function because we mostly use queue for communicating
//									//between threads. Always return a value since it would wait
//		unique_lock guard{ m };
//		data_cond.wait (guard, [this] {return !data_queue.empty (); });
//		val = data_queue.front ();
//		data_queue.pop ();
//	}
//	shared_ptr<T> wait_and_pop () {
//		unique_lock guard{ m };
//		data_cond.wait (guard, [this] {return !data_queue.empty (); });
//		shared_ptr<T> val{ make_shared<T> (data_queue.front ()) };
//		data_queue.pop ();
//		return val;
//	}
//	bool try_pop (T& val) {	//use try_pop() as a normal pop() function. Return false if there's no data
//		scoped_lock guard{ m };
//		if (data_queue.empty ())
//			return false;
//		val = data_queue.front ();
//		data_queue.pop ();
//		return true;
//	}
//	shared_ptr<T> try_pop () {
//		scoped_lock guard{ m };
//		if (data_queue.empty ())
//			return make_shared<T> ();
//		shared_ptr<T> val{ make_shared<T> (data_queue.front ()) };
//		data_queue.pop ();
//		return val;
//	}
//	bool empty () const {
//		scoped_lock guard{ m };	//Lock because the data could be affect during returning to empty()
//		return data_queue.empty ();
//	}
//};