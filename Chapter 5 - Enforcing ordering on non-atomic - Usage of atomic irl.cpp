//#include <iostream>
//#include <thread>
//#include <assert.h>
//using namespace std;
//
//bool x = false;
//atomic<bool> y;
//atomic<int>z;
//
//void write_x_then_y () {
//	x = true;	//Any place that used memory ordered relaxed can be exchange with
//				//normal data type
//	atomic_thread_fence (memory_order_release);
//	y.store (true, memory_order_relaxed);	//Store and load on y steel need to be 
//						//atomic or else there would be a data race
//}
//void read_y_then_x () {
//	while (!y.load (memory_order_relaxed));	//Use this to guarantee read to x is not UNDEFINED behavior (preventing accessing normal data type by multiple thread simultaneously
//	atomic_thread_fence (memory_order_acquire);
//	if (x)
//		++z;
//}
//
//int main () {
//	x = false;
//	y = false;
//	z = 0;
//	thread a (write_x_then_y);
//	thread b (read_y_then_x);
//	a.join ();
//	b.join ();
//	assert (z.load () != 0);
//}
//
///*
//Atomic data type guarantee access from multiple thread at once WON"T cause an undefined behavior (of course you still need to give it correct ordering)
//So by combining atomic as a FLAG, and other data fence to help separate the critical part from the non-critical part, we can create a concurrent program
//*/