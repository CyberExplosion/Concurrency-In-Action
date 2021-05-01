//#include <thread>
//#include <iostream>
//#include <vector>
//using namespace std;
//
//vector<int> queue_data;
//atomic<int> countNum;
//
//void process_data (int val) {
//	return;
//}
//
//void populate_queue () {
//	unsigned const number_of_items{ 20 };
//	queue_data.clear ();
//	for (unsigned int i = 0; i < number_of_items; i++) 	{
//		queue_data.push_back (i);
//	}
//	countNum.store (number_of_items, memory_order_release);
//}
//
//void consume_queue_item () {
//	while (true) {
//		int index;
//		if ((index = countNum.fetch_sub (1, memory_order_acquire)) <= 0) {	//RMW process, first return is 20
//									//Sine it reads the countNum b4 write, once hit 0 that will be the last data
//			this_thread::sleep_for (std::chrono::milliseconds (1));	//Sleep while waiting for refill
//			continue;
//		}
//		process_data (queue_data.at (index - 1));
//	}
//}
//
//int main () {
//	thread a{ populate_queue };
//	thread b{ consume_queue_item };	//Even with 2 threads using Acquire, the Release will synchronize with them
//	thread c{ consume_queue_item };	//both. They of course doesn't see anything in each other. However, they
//								//both will have the effect of the Release happens before (happens-before) relationship is guarantee with the release
//	a.join ();
//	b.join ();
//	c.join ();
//}
//
////NOTE: There’s still no synchronizes-with relationship between the two consumer threads.
//
///*
//the release sequence rules assures multiple threads can synchronize their loads on a single store. The synchronization in question is that of memory accesses to data other than the actual atomic variable being synchronized on (which is guaranteed to be synchronized anyway due to being atomic).
//*/
//
///*
//Good analogy: Imagine thread b synchronize with thread a because acquire resale. However, thread c then see the new countNum value WRITE by thread b (fetch_sub). But thread c and b does not synchronize. Moreover, since RMW always operetta on latest value, THERFORE you won't having 2 same index.
//	Also because c knows that it's value is written by b. And b is synchronize with a. That makes c synchronize with a
//*/
//
///*
//TLDR: You can perform multiple release on the same thread and it will synchronize with each other. 
//	You can put in MANY RMW operation (fetch_ compare_ load_) and still have them synchronize with the release
//*/