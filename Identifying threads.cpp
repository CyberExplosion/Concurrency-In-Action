//#include <iostream>
//#include <Windows.h>
//#include <thread>
//#include <unordered_set>
//using namespace std;
//
//struct sub_work {
//	void operator() () {
//		cout << "Sub Thread's id: " << this_thread::get_id () << "\n";
//	}
//};
//
//int main () {
//	thread sub{ sub_work () };
//	thread sub2{ sub_work () };
//	cout << "Sub 1 id: " << sub.get_id () << endl;
//	cout << "Sub 2 id: " << sub2.get_id () << endl;			//Apparently, the only good thing about thread id is that different threads will have different id
//															//The comparison (bigger, smaller) operator is created, but doesn't reflect if the thread is created b4 or after one another
//																// Thus, this thread id can be used in both ordered and unordered containers. But idk what good is it for ordered
//	cout << "This thread's id: " << this_thread::get_id () << "\n";
//	sub.join ();
//	sub2.join ();
//
//	std::hash<std::thread::id> thread_hash;//Could be a way to use thread's id to create key for unordered data structure like map
//	unordered_set<thread, std::hash<std::thread::id>> myThreadSet;	//This works? The books says something like this could works. Using the thread ID as a hash key, and since it's not 
//																	//ordered and the equal operation is already defined in std::thread, it'll work
//}