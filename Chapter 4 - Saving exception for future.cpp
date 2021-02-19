//#include <future>
//#include <iostream>
//#include<chrono>
//using namespace std;
//
///*
//with async, when the future call std::future.get(), if the function associated with the future throw exception, it will be handle correctly
//*/
//
//double square_root (int x) {
//	if (x < 0) {
//		throw std::out_of_range ("x < 0");
//	}
//	return sqrt (x);
//}
//
////int main () {
////	future<double> fut = async (square_root, -1);
////	double y = fut.get ();	//Error will be throw here
////}
//
////Promises require more works
//double sqrt_promise (int x, promise<double> p) {
//	double res = square_root (x);
//	p.set_value (res);
//}
//
////You can also use promise or packaged_task to deliver the exception out
//int main () {
//	promise<double> some_promise;
//	try {
//		some_promise.set_value (square_root (-1));
//	} catch (...) {
//		some_promise.set_exception (current_exception ());	//This will rethrow the exception out when someone call the future.get()
//		//Or you can also use
//		some_promise.set_exception (make_exception_ptr (std::logic_error ("smaller than 0")));	//Since set_exception only accept exception pointer, if you want a customize one you have to make it
//	}
//	future<double> some_fut{ some_promise.get_future () };	//Associate the promise with future
//
//	//Packaged_task
//	packaged_task<double (int)> some_task (square_root);
//	future<double> some_fut = some_task.get_future ();	//Associate the future with task
//	thread  th (std::move (some_task), -1);	//spawn new thread to take care of the tasks
//	//...
//	double res = some_fut.get ();
//	th.join ();	//Join thread back so no crash
//
//	//Promise on another thread
//	promise<double> sum_promise;
//	future<double> sum_future{ sum_promise.get_future () };
//	thread t (sqrt_promise, -1, std::move (sum_promise));	//Spawn new thread to work on, promise will have the return result
//	//...
//	double promise_result = sum_future.get ();
//	t.join ();
//}
//
//void work (promise<double> res) {
//	std::this_thread::sleep_for (chrono::seconds (1));	//Sleep for 1 second
//	res.set_value (0.0);
//}
//
//void demonstrate () {
//	//Using promise and future to communicate between 2 thread
//	promise<double> p;
//	future<double> f{ p.get_future () };
//	thread work_thread (work, std::move (p));	//have another thread working
//
//	//...
//	f.wait ();
//	work_thread.join ();
//}
//
////Future and packaged_task are just "provider" that need to be put on another thread to work on if you want to fully use multythread