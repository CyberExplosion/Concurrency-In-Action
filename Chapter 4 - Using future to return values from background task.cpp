//#include <future>
//#include <iostream>
//#include<string>
//using namespace std;
//
///*
//Future is use for one-off event ==> Waiting for a signal for your flight -> Once it's on, you don't have to wait again, you just go on the plane
//	\
//	You can eat, drink coffe, read books while waiting, but once the signal is on --> YOu GO = ONE OFF event
//*/
//
////You can use future to act as a thread. With a thread you need to take care of transferring result back
////With future, you can
//
//int find_answer_to_universe () { return 42; }
//void other_stuff (){}
//int main () {
//	future<int> the_answer{ std::async (find_answer_to_universe) };	//Use future and async to have a task work on background
//	other_stuff ();	//While you working on other stuffs
//	cout << "The answer is: " << the_answer.get () << endl;	//The function could just be called here or before
//}
//
////Passing arguments to a function with async
//
//struct Base {
//	void foo (int, string const&);
//	string bar (string const&);
//};
//
//Base obj;
//auto f1{ std::async (&Base::foo, &obj, 42, "hello") };	//This call p->foo(42, "hello)
//auto f2{ std::async (&Base::bar, obj, "goodbye") };	//This creates a copy of "obj" and pass it in (aka the effect not saved)
//
//struct Pill {
//	double operator()(double);
//};
//
//Pill bell;
//auto f3{ std::async (Pill (), 3.141) };	//This pass in an rvalue , which will move-constructed into Pill(3.414)
//auto f4{ std::async (std::ref (bell), 2.718) };	//This will yield bell(2.718), we pass bell function in with a wrapper, it's fine
//
//Base baz (Base&);	//Function declaration
//
//auto f6 = std::async (baz, std::ref (obj));	//Since the function above requires argument pass in as reference, have to wrap 
//	//the obj into a reference
//
//class move_only {
//public:
//	move_only () = default;
//	move_only (move_only&&) = default;
//	move_only (move_only const&) = delete;
//	void operator() ();
//};
//
//auto f5{ std::async (move_only ()) };