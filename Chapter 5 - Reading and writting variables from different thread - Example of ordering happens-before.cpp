//#include <iostream>
//#include <thread>
//#include <vector>
//using namespace std;
//
//std::vector<int> dataVec;
//atomic<bool> data_ready (false);
//
//void reader_thread() {
//	while (!data_ready.load ()) {	//Data is sure to be ready here
//		std::this_thread::sleep_until (std::chrono::high_resolution_clock::now () + std::chrono::milliseconds (1));	//Then you can read the data
//	}
//	cout << "The answer is = " << dataVec.at (0) << "\n";
//}
//
//void writer_thread () {
//	dataVec.push_back (42);	//Insert the data
//	data_ready.store (true);	//Make the data ready
//}
//
///*
//This synchronization between thread is provided by enforced ordering by the atomic_bool "data_ready"
//	With memory order relations : happens-before and synchronize-with
//	The write of the data happens BEFORE the flag
//	The read of the flag happens BEFORE the read of the data
//	\
//		When the value atomic is true, the Happens-Before relationship is created
//	The write to data -> write to flag -> read the flag -> read the data === Enforced Ordering
//
//	This enforced ordering both before and after the atomic function is due to the default memory ordering
//		\
//		Using other memory ordering might provide more efficiency
//*/