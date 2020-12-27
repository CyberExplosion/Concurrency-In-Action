//#include <iostream>
//#include <numeric>
//#include <vector>
//#include <thread>
//using namespace std;
//
////Parallel version of accumulates
//template<typename Iterator, typename T>
//struct accum_block {
//	void operator() (Iterator first, Iterator last, T& result) {	//pass by ref because thread don't have a way to return value
//		result = std::accumulate (first, last, result);
//	}
//};
//
//template<typename Iterator, typename T>
//T parallel_accum (Iterator first, Iterator last, T init) {
//	const int max_entry_each_thread{ 25 };			//Min number of entry that each thread should have
//	int length = std::distance (first, last);
//	int max_hardware_thread{ (int)thread::hardware_concurrency () };
//	int theory_max_thread{
//		(int)std::ceil ((double)(length) / max_entry_each_thread)	//Pigeon hole principle
//	};
//	int num_thread{ max_hardware_thread == 0 ? 2 : std::min(theory_max_thread, max_hardware_thread) };	//You don't want to use too many threads than what's needed and what's physically 
//																											//possible. If it's 0 then automatically assign it to 2
//	int block_size = length / num_thread;	//The number of entry in each block
//	vector<T> result (num_thread);
//	vector<thread> threads (num_thread - 1);	//Since we already in a thread right now, we need to remove 1 off
//
//	Iterator beg_block = first;
//	for (int i = 0; i < num_thread - 1; i++) {
//		Iterator end_block{ beg_block };
//		std::advance (end_block, block_size);	//Move the end iterator to be the past-the-end iterator of this block
//		threads.at (i) = thread{
//			accum_block<Iterator, T> (), beg_block, end_block, std::ref (result.at (i))
//		};
//		beg_block = end_block;	//Continue the chain
//	}
//
//	accum_block<Iterator, T> ()(beg_block, last, *result.rbegin ());	//Last calculation for the last block will be done using this thread
//
//	for (auto& entry : threads) {
//		entry.join ();
//	}
//
//	return (std::accumulate (begin (result), end (result), init));
//}
//
//int main () {
//	vector<int> num (100);
//	int init = 1;
//	for (auto& i : num) {
//		i += init;
//		init += 1;
//	}
//
//	int i = 0;
//	cout << parallel_accum<vector<int>::iterator, int> (begin (num), end (num), i);
//}