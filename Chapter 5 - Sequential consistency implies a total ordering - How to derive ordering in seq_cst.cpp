//#include <iostream>
//#include <thread>
//#include <assert.h>
//using namespace std;
//
////The default memory ordering is that of Sequential consistent ordering
////-->memory_order_seq_cst
//
///*
//If all atomic operation uses this memory ordering, the behavior of the multi threaded program act in a SIMPLE SEQUENCE like normal programming
//--> You can write all possible sequence of operations, eliminate those that inconsistent and the left over is the exact sequence the multi thread program goes
//*/
//
//atomic<bool> x, y;
//atomic<int> val{ 0 };
//
//void write_x () {	//1
//	x.store (true);
//}
//void write_y () {		//2
//	y.store (true);
//}
//
//void read_x_then_y () {	//3
//	while (!x.load ());	//Reach here means that x is true
//	if (y.load ())	//Pass this means y is true
//		++val;
//}
//void read_y_then_x () {	//4
//	while (!y.load ());
//	if (x.load ())
//		++val;
//}
//
//int main () {
//	x = false;
//	y = false;
//	val = 0;
//	thread a (write_x);
//	thread b (write_y);
//	thread c (read_x_then_y);
//	thread d (read_y_then_x);
//	a.join ();
//	b.join ();
//	c.join ();
//	d.join ();
//	assert (val.load () != 0);	//There's no way the val could stay 0. It has to be 1 if the multi thread run concurrently, or 2 if they run parallel.
//}
///*HOW TO FIGURE OUT THE SEQUENCE OF THE PROGRAM
//	Loop in 3
//		\
//		Pass -> Then check Y
//						\
//						Fail: Then x must be stored before y
//							==> Path: 1->3->2->4 result is 1
//						Pass: No conclusion
//		Fail -> Check loop in 4
//						\
//						Fail: No conclusion
//						Pass: -> Check x
//							\
//							Fail: Then y must be stored before x
//								==>Path: 2->4->1->3
//*/
////If two running at the same time and both load, then the result could be  2. But there's no way it's 0
////So this is the easiest memory order, you can deduce the order easily once you record the result
////However, this ordering requires a lot of CPU resources, and thus should be optimize later if have time