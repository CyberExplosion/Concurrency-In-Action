//#include <iostream>
//#include <thread>
//using namespace std;
//
//struct Foo {
//	void operator() () const {
//		cout << "Hey what dup!\n";
//	}
//	void do_sth ();
//	void and_another ();
//};
//
//class scoped_thread {
//private:
//	thread t;
//public:
//	explicit scoped_thread (thread arg) : t (std::move (arg)) {
//		if (!t.joinable ())	//passed in as a copy
//			throw logic_error ("No thread\n");
//	}
//	~scoped_thread () {
//		t.join ();
//	}
//	scoped_thread (const scoped_thread& rhs) = delete;
//	scoped_thread& operator=(const scoped_thread& rhs) = delete;
//};
//
//
//int main () {
//	scoped_thread{ thread{Foo()} };
//	thread t{ Foo() };
//	scoped_thread lock{ std::move(t) };
//}