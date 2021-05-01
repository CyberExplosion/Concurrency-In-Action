#include <thread>
#include <iostream>
#include <assert.h>
#include <thread>
using namespace std;

atomic<bool>x, y;
atomic<int> z;

void write_x_then_y () {
	x.store (true, memory_order_relaxed);
	atomic_thread_fence (memory_order_release);	// /=========	//Also using fence like this is 
											//exactly like putting y on memory_order_releas and later acquire
									//Maybe they like fence better because it's more easy to understand by human
	y.store (true, memory_order_relaxed);
}

void read_y_then_x () {
	while (!y.load (memory_order_acquire));
	atomic_thread_fence (memory_order_acquire); // \==========
	if (x.load (memory_order_relaxed)) {
		++z;
	}
}

int main () {
	x = false;
	y = false;
	z = 0;
	thread a (write_x_then_y);
	thread b (read_y_then_x);
	a.join ();
	b.join ();
	assert (z.load () != 0);
}

/*
with different variable, the relaxed model doesn't bring happens-before across thread or synchronize-with relationship.
Fence is like what you think, it's a way to separate and impose an ORDER
	THINGS happen before the FENCE and things comes after the FENCE
using the way above, once the fence synchronize, we can be sure that x (outside fence) happens BEFORE y (inside the fence)
*/

//Thus, if we do it like below, fence doesn't work
void faulty_write_x_then_y () {
	atomic_thread_fence (memory_order_release);
	x.store (true, memory_order_relaxed);	//Since there's no fence between x and y, there's no 
	y.store (true, memory_order_relaxed);	//happens-before across thread relationship will happen
										//once the fence synchronize
}