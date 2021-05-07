//#include <mutex>
//#include <queue>
//#include <condition_variable>
//using namespace std;
//
//template <typename T>
//class threadsafe_queue {
//private:
//	mutable mutex m;
//	queue<shared_ptr<T>> data_queue;
//	condition_variable cond;
//public:
//	threadsafe_queue(){}
//	void wait_and_pop (T& value) {	//Option 1 pass out using a provided reference.
//								//This is for guarantee that it will get a value out
//		unique_lock guard{ m };
//		cond.wait (guard, [this] () {return !data_queue.empty (); });
//		value = move (*data_queue.front ());	//Only throw if the move or copy assignment is
//												//wrong
//		data_queue.pop ();	//Even with exception, the queue won't be touched till the end
//	}
//	bool try_pop (T& value) {	//This will try to get a value out if it can
//		scoped_lock guard{ m };
//		if (data_queue.empty ())
//			return false;	//Fail to get a value out
//		value = move (*data_queue.front ());
//		data_queue.pop ();
//		return true;
//	}
//	shared_ptr<T> wait_and_pop () {	//Option 2 pass out a pointer to the value
//		unique_lock guard{ m };
//		cond.wait (guard, [this] () {return !data_queue.empty (); });
//		shared_ptr<T> res = data_queue.front ();	//No move because with share pointer it's
//											//made to copy and increment reference count
//		data_queue.pop ();
//		return res;
//	}
//	shared_ptr<T> try_pop () { //Option 2, no need for check of boolean value
//		scoped_lock guard{ m };
//		try {
//		if (data_queue.empty ())
//			return shared_ptr<T> ();	//return null ptr
//			shared_ptr<T> res = data_queue.front ();	//Here could throw if not enough mems
//			data_queue.pop ();
//			return res;
//		} catch (...) {
//			cond.notify_one ();
//			throw;
//		}
//	}
//	void push (T new_value) {	//Thread so copy
//		shared_ptr<T> data{ make_shared<T> (move(new_value)) };	//using pointer also allow us to
//				//move the declaration outside of lock and thus could increase some speed
//		scoped_lock guard{ m };
//		data_queue.push (data);
//		cond.notify_one ();	//This would not be good if there's a thread receive the signal and
//		//then throw exception when making shared_ptr. Use notify_all() if you want to be sure
//		//Or even better, make the catching exception notify_one() other thread
//	}
//};