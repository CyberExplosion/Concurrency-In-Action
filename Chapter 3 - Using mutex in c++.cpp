//#include <iostream>
//#include <mutex>
//#include <list>
//#include <algorithm>
//#include <thread>
//using namespace std;
//
//class parallel_list {
//private:
//	mutex lock;
//	list<int> nums;
//public:
//	void add_item (int item) {
//		scoped_lock<mutex> guard{ lock };	//Critical parts
//		nums.push_back (item);
//	}
//
//	bool listContains (int item) {
//		scoped_lock<mutex> guard{ lock };
//		return find (begin (nums), end (nums), item) != nums.end ();
//	}
//};