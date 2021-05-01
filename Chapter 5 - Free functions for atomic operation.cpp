//#include <iostream>
//#include <assert.h>
//using namespace std;
//
//int main () {
//	//Aside from the member function in each atomic data type, there's many atomic free functions that the std provide for us to use on many different atomic type
//	atomic<int> val;
//	std::atomic_store (&val, 5);	//The first parameter takes a pointer(=address), second takes the value to store
//	std::atomic_store_explicit (&val, 5, std::memory_order_release);	//You can use the _explicit version to add memory orders
//	int expected{ 5 };
//	std::atomic_compare_exchange_weak (&val, &expected, 0);
//	std::atomic_compare_exchange_weak_explicit (&val, &expected, 0, std::memory_order_acq_rel, std::memory_order_acquire);	  //For the explicit version of compare_exchange, you have to explicit declare BOTH the success and fail memory order (in the member function version, you only need to declare the success version and it will assume the same for the failure)
//
//	atomic_flag flag;	//since c++20 they default constructed to clear
//	assert (flag.test () == false);
//	std::atomic_flag_test_and_set (&flag);
//	std::atomic_flag_clear (&flag);
//	std::atomic_flag_clear_explicit (&flag, std::memory_order_release);
//
//	//Some how the committee thinks that the Atomic Free function can be useful to use on share_ptr class. Therefore we can use the share_ptr<T> as a parameter
//	struct my_data {};
//	shared_ptr<my_data> p;
//
//	void process_data (shared_ptr<my_data> local);
//
//	auto func = [] () -> int {	//Makign function in function
//		return 0;
//	};
//
//	//Function below is depreciated due to c++20 shared_ptr specialization makes working with atomic free function much clearer
//	// 
//	//auto process_global_data = [&] () ->void {
//	//	shared_ptr<my_data> local{ std::atomic_load (&p) };
//	//	process_data (local);
//	//};
//
//	//C++20 provide Specialization for share_ptr in atomic<T> template. Now you can make sure that it's clearer and assure all access are atomic by not forgetting to use the free atomic function
//	atomic<shared_ptr<my_data>> pointer;
//	if (pointer.is_lock_free ())
//		cout << "This is lock free for atomic shared_ptr\n";
//	else
//		cout << "It is not\n";
//
//	atomic<shared_ptr<int>> point;
//	if (point.is_lock_free ())
//		cout << "BUt this is\n";
//	else
//		cout << "Or my computer is just bad\n";
//
//}