#include <list>
#include <iostream>
#include <future>
#include <chrono>
#include <algorithm>
using namespace std;

template <typename T>
list<T> sequential_quick_sort (list<T> input) {
	if (input.empty ())
		return input;
	list<T> result;
	result.splice (result.begin (), input, input.begin ());		//Take the first value of the input as the pivot points --(cut it into the result so not waste copying around
	const T& pivot{ *result.begin () };

	auto divide_point = std::partition (input.begin (), input.end (), [&] (const auto& val) {	//Partition will return a pointer to the first value that FAILED the check --AKA first value in bigger than pivot group
			return val < pivot;		//Separate the list into smaller and bigger than pivot
		});
	list<T> lower;
	lower.splice (lower.begin (), input, input.begin(), divide_point);	//NOT including the divide_point
	//Now the input-list only have higher or equal pivot value

	//Recursion step
	auto neo_lower{ sequential_quick_sort (std::move(lower)) };
	auto neo_higher{ sequential_quick_sort (std::move(input)) };	//Uses move to not make copy
	result.splice (result.begin (), neo_lower);
	result.splice (result.end (), neo_higher);	//Put the higher at the end and the lower at the top
	return result;
}

//Parallel quick sort using thread
template <typename T>
list<T> parallel_quick_sort_async (list<T> input) {
	if (input.empty ())
		return input;
	list<T> result;
	result.splice (result.begin (), input, input.begin ());
	auto const& pivot{ *result.begin () };

	auto divide_point{ partition (input.begin (), input.end (),[&] (auto const& val) {
		return val < pivot;
}) };
	list<T> lower_half;
	lower_half.splice (lower_half.begin (), input, input.begin (), divide_point);

	//Spawn new thread to handle one half
	auto low_fut{ std::async (&parallel_quick_sort_async<T>, std::move (lower_half)) };	//This will spawn another thread, up until the system deems unsafe no more
		//If recurse 1 times, we have this thread and another thread. 3 times we have 8 threads, 3 source and 3 low_fut thread.
	auto high_part{ parallel_quick_sort_async (std::move(input)) };	//input now only have higher half

	result.splice (result.end (), high_part);
	result.splice (result.begin (), low_fut.get ());
	return result;
}

int main () {
	list<int> l;
	l.push_back (5);
	l.push_back (0);
	l.push_back (12);
	l.push_back (1);
	l.push_back (3);

	////auto res{ sequential_quick_sort (l) };
	//auto res{ parallel_quick_sort_async (l) };
	//for (const auto& val : res)
	//	cout << val << " ";
	list<int> l2;
	l2.push_back (5);
	l2.push_back (0);
	l2.push_back (12);
	l2.push_back (1);
	l2.push_back (3);

	auto start = std::chrono::high_resolution_clock::now ();
	auto res{ sequential_quick_sort (l) };
	chrono::duration<float> diff = chrono::high_resolution_clock::now () - start;
	cout << "The time sequential quick sort takes: " << diff.count() << " seconds \n";
	start = std::chrono::high_resolution_clock::now ();
	auto res2{ parallel_quick_sort_async(l2) };
	chrono::duration<float> diff2 = chrono::high_resolution_clock::now () - start;
	cout << "The time parallel quick sort takes: " << diff2.count () << " seconds\n";
}