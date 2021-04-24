//#include <iostream>
//#include <thread>
//#include <mutex>
//using namespace std;
//
////Since operation of atomic_flag is guarantee lock free and always ordered nicely with no data race --> All mutex are built from atomic
//
//class spinlock_mutex {
//private:
//	atomic_flag flag;
//public:
//	spinlock_mutex() : flag(ATOMIC_FLAG_INIT){}
//	
//	void lock (){
//		while (flag.test_and_set (std::memory_order_acquire));	//Get the lock here 3
//			//Test and set will first return the state of the flag , THEN set the flag to true
//			// So at 1st thread, while loop escape because return value is Flag = false. Before escape loop, however, flag set to TRUE
//			// Then if another thread trying to get the flag, the func will return true and keep looping ==> Spinning Lock
//			//Then it will just keep spinning until the test_and_set is false --> When we clear the flag
//	}
//
//	void unlock () noexcept {
//		flag.clear (std::memory_order_release);	//Make the flag turn false ==> Stop spinning
//	}
//};
//
////Memory Order:
///*
//The acquire order will make any 
//*/
//
//int main () {
//	mutex m;
//
//	//You can even use the spinlock above for the lock_guard<> class or the unique_lock<> class
//	std::lock_guard<spinlock_mutex> spinLck (m, std::defer_lock);
//	std::unique_lock<spinlock_mutex> spin (m, std::adopt_lock);
//}