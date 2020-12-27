//#include<iostream>
//#include <mutex>
//using namespace std;
//
////DEADLOCK happen when trying to use multiple mutex
//
//
////Best way to avoid deadlock when locking multiple mutexes is to lock them in the SAME ORDER ALWAYS
////A first then B
//
//struct some_big_data {
//
//};
//
//void swap (some_big_data& lhs, some_big_data& rhs);
//
//class protect_data {
//private:
//	some_big_data data;
//	mutex m;
//public:
//	protect_data (some_big_data& sd) : data (sd) {}
//	friend void swap (protect_data& lhs, protect_data& rhs) {
//		if (&lhs == &rhs)	//Have to check for aliasing, if both data are the same we will end up creating 2 locks
//			return;			//object for the same lock m, which is illegal
//
//		scoped_lock guard{ lhs.m, rhs.m };		//Scope lock allow you to lock 2 different lock at the same time
//														//Thus, preventing deadlock
//
//		swap (lhs.data, rhs.data);		//Better semantic to create a swap function for your class is to use existing
//										//one outside and thus don't need to call create a member function
//										//AKA: no need for: obj.swap()	<--- Ugly
//	}
//};
//
//int main () {
//	//Deadlock example
//	/*
//	Class{
//		data
//		lock
//	}
//	Class object 1, object 2
//	object 1. lock1()		object 2. lock2();
//		Thread 1										Thread 2
//	swap(object1, object2)						swap(object2, object1)		--> Reverse order of locking
//		object1.lock1()								object2.lock2()
//		object2.lock2()								object1.lock1()
//			\Can't, waiting thread 2 release			\Can't waiting thread 1 release lock1
//			lock2
//							DEADLOCK
//	*/
//}