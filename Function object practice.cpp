//#include <iostream>
//using namespace std;
//
////Function object is a way to create function that can change its' easy behavior each time you call it
////Example
//
//class older_than {
//private:
//	int limit{ 0 };
//public:
//	older_than (int l) : limit (l) {};
//	void operator() () const {
//		cout << "There's nothing here\n";
//	}
//	void operator() (const int& num) const {
//		if (num > limit)
//			cout << "This ain't it chief\n";
//		else
//			cout << "Ok inside limit\n";
//	}
//};
//
//int main () {
//	older_than olderThan60 (60);	//create a function here
//	olderThan60 ();
//	older_than olderThan90 (90);	//create a familiar function with different action here	
//	olderThan90 (50);
//}