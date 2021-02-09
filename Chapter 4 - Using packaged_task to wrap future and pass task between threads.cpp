//#include <future>
//#include <deque>
//#include <mutex>
//#include <thread>
//#include <iostream>
//using namespace std;
//
////Will use a GUI model to demonstrate how to managing tasks with future
//
//mutex m;
//deque<packaged_task<void ()>> Tasks;
//bool gui_shutdown_msg_received ();
//void get_and_process_gui_msg ();
//
//bool gui_thread () {
//	while (!gui_shutdown_msg_received ()) {
//		get_and_process_gui_msg ();
//		if (Tasks.empty ())
//			continue;
//		packaged_task<void ()> task;
//		{
//			scoped_lock locker{ m };
//			task =  std::move (Tasks.front ()) ;	//packagaed_task can only be moved, not copyable
//			Tasks.pop_front ();
//		}
//		task ();	//Execute the task ==> Make the function inside task start running
//	}
//}
//
//thread gui_background_thread;
//template <typename Func>
//future<void> put_task_for_gui_thread (Func f) {	//This function will post the task onto the Tasks list ---> AKA put the function in a list waiting to be executed
//	packaged_task<void ()> task{ f };
//	future<void> res{ task.get_future () };
//	scoped_lock locker{ m };
//	Tasks.push_back (std::move(task));
//	return res;	//Return the associative future so that we can get the task result later
//}
//
//packaged_task<int (string&, int)> complicated_task;		//This can also be use for a function of type int(string&, int), this will associate with a future of type future<int>
//	//There are many way we can wrap complicated functions into packaged_task, so that we can simplify the FUTURE returns object
//
////A task is a perfect wrapper to use on another thread. When that thread finish, the future will be ready
//int foo (double);
//packaged_task<int (double)> task (foo);
//auto fut = task.get_future ();
//
//thread th (std::move (task), 1.5);	//Move the task into a thread, perfect for managing
//
////... a few moment later
//auto result = fut.get ();	//After the thread finish, the task also finish. Thus the future also Finish