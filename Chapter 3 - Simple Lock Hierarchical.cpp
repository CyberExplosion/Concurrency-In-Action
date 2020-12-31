//#include <iostream>
//#include <mutex>
//using namespace std;
//
////In a hierarchical mutex, thread with high level lock can "acquire" low level lock, 
////Low level lock CANNOT "acquire" high level lock
//
////This hierarchy lock can help you to prevent lock from a user function
//
//class Hierarchical_mutex {
//	mutex internal_lock;
//	unsigned long const try_hiearchy_val;	//New hierarchy value that you trying to get
//	unsigned long prev_hiearchy_val;
//	static thread_local unsigned long local_hierarchy_value;	//Thread local requires static when inside class
//													//Using thread local ensures each threads has their own number
//					//Thus, this would be use as the hierarchal value for each threads
//					//Also static cannot be initialized inside class
//	void checkHierarchyViolation () {
//		if (try_hiearchy_val >= local_hierarchy_value) {	//Trying to get a higher rank lock
//			throw logic_error ("mutex hierarchy violated");
//		}
//	}
//	void updateHierarchyValue () {
//		prev_hiearchy_val = local_hierarchy_value;
//		local_hierarchy_value = try_hiearchy_val;
//	}
//
//	//To use any scope_lock or lock<type T> for any custom lock type, you need to define these 3 functions below
//public:
//	explicit Hierarchical_mutex (unsigned long val) : try_hiearchy_val (val), prev_hiearchy_val (0) {}
//
//	void lock () {
//		checkHierarchyViolation ();
//		internal_lock.lock ();
//		updateHierarchyValue ();	//Only update the hierarchy once everything is ok
//	}
//	void unlock () {
//		if (local_hierarchy_value != try_hiearchy_val)	//Trying to unlock a wrong or out of order lock
//			throw logic_error ("mutex hierarchy not correct");	//To prevent somehow that we might unlock wrong hierarchy
//		local_hierarchy_value = prev_hiearchy_val;	//Return the prev hierarchy back so we can lock again
//		internal_lock.unlock ();
//	}
//	bool try_lock () {
//		checkHierarchyViolation ();
//		if (!internal_lock.try_lock ())
//			return false;
//		updateHierarchyValue ();	//Since the above check pass, we update the hierarchy with the new value
//		return true;
//	}
//};
//thread_local unsigned long Hierarchical_mutex::local_hierarchy_value{ ULONG_MAX };	//Assign the highest value at the beginning. So that initially, any lock created can hold any thread
//
//
//Hierarchical_mutex high_lv_lock{ 10000 };
//Hierarchical_mutex low_lv_lock{ 5000 };
//Hierarchical_mutex other_lv_lock{ 6000 };
//
//int low_level () {
//	return 0;
//}
//int low_lv_func () {
//	
//	scoped_lock<Hierarchical_mutex> guard{ low_lv_lock };	//Create a guard for our low level lock
//	return low_level ();
//}
//
//void high_level (int some_thing){}
//void high_lv_func () {
//	scoped_lock<Hierarchical_mutex> guard{ high_lv_lock };	//Create a guard for our high level lock
//	high_level (low_lv_func ());	//This here will call the low level function, which then will trigger
//									// another lock
//									//This could violate the "Avoid calling another function while holding a lock"
//							// Since if another function waiting the same lock we holding, we have deadlock
//			//But we using a lock hierarchy, thus enforcing an one-way lock traversing order and eliminating deadlock (only higher to lower allow)
//}	//In this case, since it's from high to low, it's allowed
//
//void thread_a () {
//	high_lv_func ();	//Good
//}
//
//void other_stuff (){}
//void other_func () {
//	high_lv_func ();
//	other_stuff ();
//}
//
//void thread_b () {
//	scoped_lock<Hierarchical_mutex> guard{ other_lv_lock };
//	other_func ();	//Exception, since this function include the high_lv_func(), which locks a higher hierarchy
//				//lock, you cannot use the other_lv_lock
//}
//
//int main () {
//	thread t1{ thread_a };
//	try {
//		thread t2{ thread_b };
//	} catch (const exception& e) {
//		cout << e.what ();
//	}
//}
