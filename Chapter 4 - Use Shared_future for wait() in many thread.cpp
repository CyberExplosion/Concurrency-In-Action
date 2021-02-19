//#include <iostream>
//#include <thread>
//#include <future>
//using namespace std;
//
////Normally, we would use <future> to create and manage a thread to communicate between
//promise<void> p1;
//future<void> fut1 = p1.get_future ();
//void func1 () {
//	p1.set_value ();	//Make ready here
//};
//
//auto res = async (std::launch::async, func1);	//Create a new thread to run func1
//
//void master_one () {
//	fut1.wait ();	//Main thread will blocks until func1 is finish
//}
//
////However, wait() or get() only works when we only have 1 individual thread that is waiting ===> Above only master_one() is the is the thread that waits on fut1
////Using shared_future, we can have multiple threads waiting on ONE future and PROVIDE result for all other threads
//	//\
//	//	With future, once 1 thread call get(), there's no more result to get ==> With Share_future, return values will be PROVIDED to all threads
////		As long as each thread has its' own shared_future obj;
//
//int main () {
//	promise<void> p;
//	future<void> fut = p.get_future ();
//	shared_future<void> sf{ std::move (fut) };	//Has to move in, Future only allows MOVE
//	//or you can do shared_future sf {p.get_future()}	Move a future using rvalue
//	//Or auto sf {p.get_future().share()}	share() function also create a share future really easy and allow you to use AUTO
//}
//
////With shared_future, you can have multiple threads waiting on the same function-future
//promise <void> p;
//auto sh_fut{ p.get_future().share() };
//
////Each local function needs to have its own copy of the shared future to prevent DATA RACE
//void func1 () {
//	auto local{ sh_fut };		//The return type need to be COPYABLE also
//	//....
//
//	local.wait ();
//}
//
//void func2 () {
//	auto local{ sh_fut };
//	//...
//
//	local.wait ();		//Now we will wait on 2 seperate <shared_future> object, prevent data race
//}
//
//auto res1{ async (std::launch::async, func1) };
//auto res2{ async (std::launch::async, func2) };	//Now we can have 2 threads waiting on the same future ---> AKA waiting on another thread maybe
//
////Share_future only allow getting result MULTIPLE Times in MULTIPLE Threads - you still have to handle data race between multiple threads trying to get <shared_future> object