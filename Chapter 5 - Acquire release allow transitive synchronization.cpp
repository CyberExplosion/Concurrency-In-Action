//#include <iostream>
//#include <thread>
//#include <assert.h>
//using namespace std;
//
//atomic<int> arr[5];
//atomic<bool> sync1 (false), sync2 (false);
//
//void thread_1 () {
//	arr[0].store (42, memory_order_relaxed);
//	arr[1].store (97, memory_order_relaxed);
//	arr[2].store (71, memory_order_relaxed);
//	arr[3].store (-141, memory_order_relaxed);
//	arr[4].store (2003, memory_order_relaxed);
//	sync1.store (true, memory_order_release);	//This here will impose the synchronize-with relationship with the acquire thread
//}
//
//void thread_2 () {
//	while (!sync1.load (memory_order_acquire));
//	sync2.store (true, memory_order_release);	//Open another synchronize-with relationship
//}
//
//void thread_3 () {
//	while (!sync2.load (memory_order_acquire));
//	assert (arr[0].load (std::memory_order_relaxed) == 42);
//	assert (arr[1].load (std::memory_order_relaxed) == 97);
//	assert (arr[2].load (std::memory_order_relaxed) == 91);
//	assert (arr[3].load (std::memory_order_relaxed) == -141);
//	assert (arr[4].load (std::memory_order_relaxed) == 2003);
//}
//
////thread 1 has the imposing happens-before order. All the value is assigned before thread 1 open the synchronize with relationship. Then thread 2 synchronize with thread 1 and continue it's own ordering. Then open to thread 3 and so on.
//
////You can even shorten thread 2 using a read_and_modify operation so that we only need 1 variable to signal the synchronize
//atomic<int> sync (0);
//void thread_1_neo () {
//	arr[0].store (42, memory_order_relaxed);
//	arr[1].store (97, memory_order_relaxed);
//	arr[2].store (71, memory_order_relaxed);
//	arr[3].store (-141, memory_order_relaxed);
//	arr[4].store (2003, memory_order_relaxed);
//	sync.store (1, memory_order_release);	//This here will impose the synchronize-with relationship with the acquire thread
//}
//
////Also we use the memory_order_acq_rel because the compare is read-and-modify function. Noted: function like fetch_or, fetch_sub() ... (those that read modify write, should have seq_cst or acq_rel memory) since the normal acquire and relase doesn't do anything on read-write, only solo read or write operation
//void thread_2_neo () {
//	int expected{ 1 };	//If sync equal to 1, then it means we have sync with the thread-1
//	while (!sync.compare_exchange_weak (expected, 2, memory_order_acq_rel) && expected != 2) {	//If sync is already 2, then expected will be assigned 2. That's why we put expected != 2 so we can
//																							//escape the loop early if sync is already 2 and makes expected to 2.  It's just a good practice
//		expected = 1;	//Reassign expected cause the compare will change it if return false. We fishing for a return TRUE here
//	}
//}
//
//void thread_3_neo () {
//	while (sync.load (memory_order_acquire) < 2);	//This way we make sure that we only wait for the thread_2 signal
//	assert (arr[0].load (std::memory_order_relaxed) == 42);
//	assert (arr[1].load (std::memory_order_relaxed) == 97);
//	assert (arr[2].load (std::memory_order_relaxed) == 91);
//	assert (arr[3].load (std::memory_order_relaxed) == -141);
//	assert (arr[4].load (std::memory_order_relaxed) == 2003);
//}