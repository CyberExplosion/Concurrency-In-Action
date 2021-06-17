#include <iostream>
#include <future>
#include <condition_variable>
#include <chrono>
#include <mutex>
using namespace std;

void time () {
	using namespace std::chrono_literals;
	auto one_day = 24h;
	auto half_an_hour{ 30min };
}

void duration () {
	chrono::milliseconds ms (54802);
	chrono::seconds s{ chrono::duration_cast<chrono::seconds>(ms) };
		//Cast milisecond into seconds, it will truncate tho, this time tis 54sec
	cout << chrono::milliseconds (1234).count ();	//Print 1234
}

int some_task () { return 0; };

void fut_wait_for () {
	future<int> f = std::async (some_task);
	if (f.wait_for (chrono::milliseconds (35)) == future_status::ready) {	//future_status::ready is return value of wait for.
		//do_something(f.get())
	}
}

//s

//Using timeout to FORCE a thread to only have a MAXIMUM Runtime
bool done{ false };
mutex m;
condition_variable cv;

//DO NOT use wait_for() if you want to have a ABSOLUTE MAXIMUM amount of time to wait for thread
/*
Reason: wait_for(3ms) will wait 3milisecond, but at the 1milisecond a wakup up happene --> BUt it's a spurious wake up, so thread went back to waiting
	\
	Lose time and doesn't provide an ABOSLUTE MAXIMUM time to wake up --> wait_until() is the default timeout one
*/
void wait_loop () {
	auto timeout{ chrono::steady_clock::now () + chrono::milliseconds (500) };	//ONly allow the thread to wait for 500ms
	unique_lock guard{ m };
	while (!done) {	//Wait until some other thread trigger DONE flag (aka just like in normal wait() predicate)
		if (cv.wait_until (guard, timeout) == std::cv_status::timeout)	//If timeout is reached, then exit
			break;
	}
		//You can also use the predicate version so that you don't have to use the while() loop
	cv.wait_until (guard, timeout, [&] () {return done; });		//Wait until time runout, or until the condition is good
}

void do_sth () {};
//Using system clock to check thread run time
void check_time () {
	auto start{ chrono::high_resolution_clock::now () };
	do_sth ();
	auto end{ chrono::high_resolution_clock::now () };
	const auto diff{ end - start };
	using seconds = chrono::duration<float, ratio<1, 1>>;
	cout << "do_sth takes " << chrono::duration<float, ratio<1,1>> (end - start).count () << " seconds";
}

int main () {
	check_time ();
}