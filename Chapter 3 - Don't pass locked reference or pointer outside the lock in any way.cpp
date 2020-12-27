//#include <iostream>
//#include <mutex>
//using namespace std;
//
//class some_data {
//	int num;
//	string name;
//public:
//	void doSth ();
//};
//
//class protect_data {
//	some_data data;
//	mutex m;
//public:		//BELOW IS 3 BIG NO NO in Using lock design and should always watched out
//	//DONT return any pointers or address outside of the lock
//	template<typename Function>
//	void process_data (Function func) {
//		scoped_lock<mutex> guard{ m };
//		func (data);	//Mess up here, they can get the data reference or pointer out using user supplied function
//	}
//
//	some_data* process_data_prob2 () {
//		scoped_lock<mutex> guard{ m };
//		some_data* ptr = &data;
//		return ptr;		//Mess up here, the outside will be able to have access to our data using this pointer
//	}
//
//	some_data& process_data_prob3 () {	//This will return the data's reference and thus allow outside to use
//		scoped_lock<mutex> guard{ m };
//		return data;
//	}
//};
//
//some_data* unprotected;
//void malicious_func (some_data& protected_data) {
//	unprotected = &protected_data;	//This function will be able to get the reference of the data out - expose
//}
//
//int main () {
//	protect_data p;
//	p.process_data (malicious_func);	//Get the protected_data out into the unprotected
//	unprotected->doSth ();	//Right here will cause race condition, since we outside of the lock
//}