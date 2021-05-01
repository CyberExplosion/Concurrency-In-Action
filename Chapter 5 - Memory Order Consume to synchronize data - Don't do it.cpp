//#include <iostream>
//#include <thread>
//#include <assert.h>
//using namespace std;
//
////DO NOT use consume memory order on normal coding, use acquire instead
//
////Consume is like Acquire, it synchronize with release. HOWEVER, it only perform synchronize-with relationship with data that DEPENDENCE on it.
//
//struct X {
//	int i;
//	string s;
//};
//
//atomic<X*> point;
//atomic<int> val;
//
//void create_X () {
//	X* dump = new X;
//	dump->i = 42;
//	dump->s = "hello";
//	val.store (99, memory_order_relaxed);
//	point.store (dump, memory_order_release);
//}
//
//void use_x () {
//	X* x;
//	while (!(x = point.load (memory_order_consume))) {
//		this_thread::sleep_for (std::chrono::microseconds (1));
//	}
//	assert (x->i == 42);	//These are depends on point, and thus will never fire
//	assert (x->s == "hello");
//	assert (val.load (memory_order_relaxed) == 99);	//The assert here may fire. Because val is not a DEPENDENCE of "point", so it will not have the synchronize-with relationship and thus may not have the correct happens-before relationship
//}
//
//int main () {
//	thread t1 (create_X);
//	thread t2 (use_x);
//	t1.join ();
//	t2.join ();
//}