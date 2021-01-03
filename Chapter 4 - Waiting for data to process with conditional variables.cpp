//#include <iostream>
//#include <mutex>
//#include <queue>
//#include <condition_variable>
//using namespace std;
//
//mutex m;
//queue<int> data_queue;
//condition_variable data_cond;
//
//bool more_data_to_prep () {
//	return true;
//}
//
//int prepare_data () {
//	return 0;
//}
//
//void data_prep_thread () {
//	while (more_data_to_prep ()) {	//Check if we can push more data into the queue
//		int const data = prepare_data ();
//		{
//			scoped_lock guard{ m };		//We put the lock into a scope, so that the moment we finish putting the data
//			data_queue.push (data);	// on it will unlock. This would allow another thread to get the lock right when
//		}	//--> (Or we can just use unlock() here, it's the same)
//		data_cond.notify_one ();	//we notify them here. If don't, the other thread has to wait till the end of the function
//	}
//}
//
//bool is_last_chunk (int d) {
//	return true;
//}
//
//void process (int da) {//SOMTHING
//}
//
//void data_processing_thread () {
//	while (true) {
//		unique_lock guard{ m };
//		data_cond.wait (guard, [] {return !data_queue.empty (); });	//If the data is NOT empty (true), then the 
//			//thread can continue. If it's EMPTY (false), then the thread will be put to sleep and can only WAKE UP 
//			//by a notify_one() or notify_all()
//			//After it wakes up, it will check the condition one more time and go to sleep again if the condition still false
//		int data = data_queue.front ();
//		data_queue.pop ();
//		guard.unlock ();	//Unlock here since we already pass the critical part of the program, should not lock more
//						//than we need to
//		process (data);
//		if (is_last_chunk (data))	//This thread will keep working on data popping out until last one
//			break;
//	}
//}