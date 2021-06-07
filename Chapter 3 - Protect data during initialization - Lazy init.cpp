#include <iostream>
#include <mutex>
#include <vector>
using namespace std;

//Lazy init example
shared_ptr<vector<int>> ptr_to_resource;	//Create but not need resource yet so don't initialized

void lazy () {
	if (!ptr_to_resource) {	//Only create or initialized the object when you need it
		ptr_to_resource.reset (new vector<int>(100));	//Need resource now so only init here
	}
	//ptr_to_resource->doSomething ();	//Do somthing after initialized
}

//First attempt to create thread safe lazy init
mutex resource_m;
void lazyFirst () {
	unique_lock guard{ resource_m };	//Problem here, every thread will have to stop here for others to do the check

	if (!ptr_to_resource)		//Unneeded stopping calls for better solution
		ptr_to_resource.reset (new vector<int> (100));
	guard.unlock ();
	//ptr_to_resource->doSomething		//
}

void undefined_behavior_lazy_double_locking () {
	if (!ptr_to_resource) {		//This is bad because imagine while one thread is creating resources, this check would not hit
		scoped_lock guard{ resource_m };
		if (!ptr_to_resource) {
			ptr_to_resource.reset (new vector<int> (100));	//Creating resources here
		}
	}
	//ptr_to_resource->doSomething()	//While resources is being created, you would doSth to it.
							//Interact on resources that hasn't finished creating is bad
}

//C++ provides std::call_once and std:once_flag for situation that ONLY 1 thread need to do something
//std::call_once is used to signal to thread to run a function or not depend on if the flag is set
//std::once_flag hold the flag. It activated if one thread using the call_once function has complete the call
//This is better way than using mutex to protect interaction
// IT"S BEST USED for data that's READ-ONLY: Need only to create once and every thread can interact with it at the same time
once_flag resource_flag;
void init_resource () {	//Create a function just for init because it would be very long if writing inside call_once's parameters
	ptr_to_resource.reset (new vector<int> (100));
}
void thread_safe_lazy () {
	call_once (resource_flag, init_resource);	//Need to pass in the flag to activate it, and also the callable
	//ptr_to_resource->doSSomting()
}

//Also works inside a class
struct connection {};
class X {
private:
	connection* gate;
	once_flag connection_init_flag;
	void open_connection () {
		gate = new connection ();
	}
public:
	void send_data (int const& data) {	//Initialization can be done here
		call_once (connection_init_flag, &X::open_connection, this);	//std and other function require passing in
														//the object pointer "this" and its' function
		//Send data here
	}
	void receive_data (int const& data) {	//Or initialization can be done here
		call_once (connection_init_flag, &X::open_connection, this);
		//RECEIVED data here
	}
};

//Static data member are initialized thread safe and CAN BE USE AS ALTERNATIVE TO std::call_once
//If you want to have a global instance
class my_class {};
my_class& get_my_class_instance () {	//Could also be use as an example for Singleton
	static my_class instance;	//This is thread safe and multiple threads can run this function and use the
	return instance;		//static member safety
}

//STATIC and CALL_ONCE are the same. However, with static you can only call it in one place. Using call_once, we have the associate once_flag to check and thus allow us to use call_once multiple time in our code
//To clarify: Using static --> Your program initialization path is well define -> click this - then do this- then initialize - then continue
	//	Using call_once --> Your program init path is not well define , like in User interface app --> click this - if not init then do it --> do thing ==> another app, if not init ..
//ex above send_data and received_data both can initialize the connection, which sometime is a good thing
//if you use static, you have to determine which function is the one that allow to open a connection