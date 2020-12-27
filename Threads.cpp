////#include "pch.h" // Use with pre-compiled header
//#include <thread>
//#include <iostream>
//#include <vector>
//#include <string>
//#include <mutex>
//#include <condition_variable>
//#include <queue>
//#include <future>
//#include <execution>
//#include <algorithm>
//using namespace std;
//
//
//int main() {
//	vector<int> number(1'000'000, 1);
//	
//	auto time1 = chrono::high_resolution_clock::now();
//	int sum = accumulate(begin(number), end(number), 0);
//	auto time2 = chrono::high_resolution_clock::now();
//	chrono::duration<double, std::milli> remain_time = time2 - time1;
//	cout << "Time it takes to sum using normal non-threaded synchronous function: " << remain_time.count() << "\n";
//
//	time1 = chrono::high_resolution_clock::now();
//	
//	sum = reduce(execution::par_unseq, begin(number), end(number));	//It's exactly like accumulate, but better
//	
//	time2 = chrono::high_resolution_clock::now();
//	chrono::duration<double, std::milli> rr = time2 - time1;
//	cout << "Time it takes to sum using multi threaded and vectorization (parallel): " << rr.count() << "\n";
//	//
//
//
//	time1 = chrono::high_resolution_clock::now();
//
//	sum = reduce(execution::par, begin(number), end(number));	
//	
//	//auto fut = async(launch::async);
//	//auto fut2 = async(launch::deferred);
//
//	time2 = chrono::high_resolution_clock::now();
//	chrono::duration<double, std::milli> rt = time2 - time1;
//	cout << "Time it takes to sum using multi threaded only: " << rt.count() << "\n";
//
//
//}