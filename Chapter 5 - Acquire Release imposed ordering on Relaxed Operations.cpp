//#include <iostream>
//#include <thread>
//#include <assert.h>
//using namespace std;
//
//atomic<bool>x, y;
//atomic<int>z;
//
////Acquire/Release model makes it looks like working with a mutex lock
//
//void write_x_then_y () {
//	x.store (true, std::memory_order_relaxed);
//	y.store (true, std::memory_order_release);	//You release the lock here ==> Meaning that everything above this is the CRITICAL SECTION of the program
//}
//
//void read_y_then_x () {
//	while (!y.load (std::memory_order_acquire));	//Acquire the lock here ==> Start dealing with CRITICAL SECTION of the program
//	if (x.load (std::memory_order_relaxed))
//		++z;
//}
//
//// When you release the lock, the acquired thread will SEE all the data before the release.
//// REMEMBER, the most important when dealing with atom is the relationship of happens-before synchronize-with
//// With memory_order_relaxed, there's only a happens-before relationship in the same thread. There's no synchronize-with guarantee so that different thread can see the correct ordering (the correct happens-before). 
//// The acquire will synchronize with the released from different thread when dealing with the same atom data. In turn, it will also synchronize the happens-before relationship with the release thread and created an order
//
//
////WARNING: acquire-release only impose the synchronizing-with relationship
////Ex:
//void write_x () {
//	x.store (true, memory_order_release);
//}
//void write_y () {
//	y.store (true, memory_order_release);
//}
//void read_x_then_y () {
//	while (!x.load (std::memory_order_acquire));
//	if (y.load (std::memory_order_acquire))
//		z++;
//}
//
//void wrong_relationship () {
//	x = false;
//	y = false;
//	z = 0;
//	thread a (write_x);
//	thread b (write_y);
//	thread c (read_x_then_y);
//	thread d (read_y_then_x);
//	a.join ();
//	b.join ();
//	c.join ();
//	d.join ();
//	assert (z.load() != 0);		//in this case this assert could fail
//}
//
///*
//* The case above could fire the assert.
//* The problem with the above is that even with the acquire/release providing synchronize-with relationship between thread. There's no happens-before order because each function runs on different thread
//*/
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
///* Acquire/release works well with relaxed because relax only offer happens-before relationship inside each thread, however, combining with acquire/release, we can get the synchronize-with and have an ordering in our program.
//* 
//* Combining happens-before with synchronize-with between threads, we get 
//*	inter-thread happens before relationship
//* 
//*/