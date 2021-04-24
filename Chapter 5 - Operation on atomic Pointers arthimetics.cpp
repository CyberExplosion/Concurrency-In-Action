//#include <iostream>
//#include <assert.h>
//using namespace std;
// 
// Warning: Atomic member functions compare_exchange may not works well with float and double data type due to the way their bit organize. Atomic compare use bitwise comparison and most of the time it's really bad with float and double data type
//
//int main () {
//	//with every atomic type, you can use the follow function to see if it's REALLY atomic operation or it's FAKE-ing using a mutex lock
//	atomic<bool> b;
//	if (b.is_lock_free ()) {
//		cout << "atomic boolean is always lock free on this machine\n";
//	} else
//		cout << "atomic boolean is sometimes or never lock free (using a mutex to fake) on this machine\n";
//	
//		//You can use many arithmetic function with atomic pointer to perform pointer arithmetics
//	class Foo {};
//	Foo some_array[5];
//	atomic<Foo*> p (some_array);	//Like others atomic, it cannot be copy-constructible or coopy-assignable. Meaning it cannot be copy from others ATOMIC value. It can still construct from normal data type
//	Foo* x = p.fetch_add (2);	//fetch_add() is a read-modify-write operation
//		//It will return the some_array[0], then do pointer arithmetics +=2.
//		//So now p will points to some_array[2]
//	assert(x == some_array);	//x gets the return value of some_array[0] so its' still valid
//	assert (p.load () == &some_array[2]);	//.load() is a read() operation, and since it's return a pointer (= address of a memory)
//	x = (p -= 1);	//Operator -=1 is equal to fetch_sub(1), without returning the old result part
//		//Now both x and p will point to asome_array[1]
//	assert (x == &some_array[1]);
//	assert (p.load () == &some_array[1]);
//
//	//You can also specified the memory order
//	p.fetch_add (3, std::memory_order_release);	//This is a read-modify-write
//
//	//Of course, integral atomic data type has all the above, and some more
//	atomic<int> val;
//	if (val.is_lock_free()) {
//		cout << "atomic integer is always lock free on this machine\n";
//	} else
//		cout << "atomic boolean is sometimes or never lock free (using a mutex to fake) on this machine\n";
//	atomic<int> test{ 0 };
//	test++;	//
//	assert (test.load () == 1);
//	int old_val = test.fetch_add (10);
//	assert (old_val == 1);
//	assert (test.load () == 11);
//	int expected{ 11 };
//	if (test.compare_exchange_weak (expected, 0)) {
//		cout << "Machine doesn't need loop for compare_exchange_weak\n";
//	} 	else
//		cout << "Need loop for compare_exhchange_weak\n";
//	cout << test.load (std::memory_order_acquire);	//Think of it's as a lock
//		//acquire = acquire lock to read	release = release lock to write
//}