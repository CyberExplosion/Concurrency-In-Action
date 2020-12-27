//#include <iostream>
//#include <thread>
//using namespace std;
//
////Using function object
//class background {
//public:
//	void operator() () const {
//		//sth
//	}
//	void do_something ();
//	void something_else ();
//};
//
//
//int main () {
//	thread t{background()};	//using function object
////Using lambda
//	thread my_thread ([] {
//		void do_something ();
//		void something_else ();
//		});
//	t.join ();
//	//my_thread.join ();
//
//	//Just for fun
//	unique_ptr<int> smart{ make_unique<int> (5) };
//}