//#include <iostream>
//#include <mutex>
//using namespace std;
//
//struct ProtectedData {
//	int data{ 0 };
//	mutex m;
//	ProtectedData () = default;
//	ProtectedData (int d) : data (d) {};
//};
//
//
////The gateway class will be the one handle all the threads trying to access the data
//class GateWay {
//	ProtectedData* pathToData;
//public:
//	GateWay (ProtectedData& data) {
//		pathToData = &data;
//	}
//	unique_lock<mutex> getLock () {
//		unique_lock<mutex> guard{pathToData->m, defer_lock};	//Establish guard to a data
//		//Prepare something for data
//		return guard;	//This will move the guard out
//	}
//
//	void process_data () {
//		unique_lock<mutex> lk (getLock());
//		//Do something to data
//	}
//};
////When an instanced of the class destroy, the lock of that instance is released and allow other threads to acquire
