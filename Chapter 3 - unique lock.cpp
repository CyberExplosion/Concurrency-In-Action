//#include <iostream>
//#include <mutex>
////using namespace std;
//
//class SomeObj{};
//
//void swap (SomeObj& lhs, SomeObj& rhs){}
//
//class X {
//	SomeObj data;
//	std::mutex m;
//public:
//	X (const SomeObj& rhs) : data (rhs) {};
//	friend void swap (X& lhs, X& rhs) {
//		if (&lhs == &rhs)
//			return;
//		//If you want to lock all at the same time, but cannot use scoped lock because it's not available yet.
//		//Then unique lock would allow you the flexibility
//		std::unique_lock guard_a{ lhs.m, std::defer_lock };	//defered_lock establish the connection between the 
//		std::unique_lock guard_b{ rhs.m, std::defer_lock };	//smart lock object and the mutex, but let it UNLOCK --> Therefore not Own the lock yet
//		std::cout << guard_a.owns_lock () << " " << guard_b.owns_lock () << std::endl;
//		//Do something
//		lock (guard_a, guard_b);	//Now we can lock both at the same time
//		std::cout << guard_a.owns_lock () << " " << guard_b.owns_lock () << std::endl;
//
//		swap (lhs.data, rhs.data);
//
//		//Since smart lock provides us with the RAII power and let us forget about locking responsibility, it's 
//		//Always recommend to wrap our lock around at least a unique_lock object with defered_lock behavior b4 we 
//		//plan to do something
//	}
//	//So this is good when yOU DONT NEED TO LOCK JUST YET
//	//ALSO good whne you want to transfer lock ownership, since scoped lock don't allow move
//};
//
//int main () {
//	SomeObj obj1, obj2;
//	X ob1 (obj1);
//	X ob2 (obj2);
//	swap (ob1, ob2);
//}