//#include <iostream>
//#include <vector>
//#include <mutex>
//using namespace std;
//
////Lock prevents thread to work on a process and thus you only use lock for a MINIMUM AMMOUNT of time required
////EX: I/O Files takes a really long time comparing to I/O from memory
////	--> Load the file into memory, release the lock, then work on it
//mutex m;
//
//void get_and_process_data () {
//	unique_lock<mutex> guard{ m };
//	//Reading file into a vector (into anything in memory)
//	guard.unlock ();	//Now only need to work using the vector, release the lock
//	//Process the data in vector
//	guard.lock ();	//Require a lock to write
//	//Write the result back into the file	
//}
//
////You can also make one for comparison between integer
////Int are cheap to copied and thus would be faster to copied them into another memory and do stuff with them
//class Y {
//	vector<int> vec;
//	mutex m;
//	int getFirstInt () const {
//		scoped_lock guard{ m };	//Only use a lock to copy data outside
//		return vec.at (0);
//	}
//public:
//	Y (int num) {
//		vec = vector<int> (1, 0);
//	}
//	friend bool operator==(Y const& lhs, Y const& rhs) {
//		if (&lhs == &rhs)
//			return true;
//		int const lhs_val = lhs.getFirstInt ();
//		int const rhs_val = rhs.getFirstInt ();
//		return lhs_val == rhs_val;
//	}
//};
//
////This, HOWEVER, change the meaning of the equal comparison
////Since it's only protected during it's copied out function, the left and right value in the end could have been different.
//	//--> Therefore this function only said: "At some point in time, value on lhs and rhs WERE EQUAL. But maybe not now"
//	//This changes the meaning of equal comparison to "maybe".	-->this is a race condition, because the result could have been change in the end
//
////THERES only 1 truth thing, IF YOU DON"T HOLD A LOCK FOR THE ENTIRE DURATION OF OPERTIOn, YOU WILL HAVE RACE CONDITION
//
//int main () {
//
//}