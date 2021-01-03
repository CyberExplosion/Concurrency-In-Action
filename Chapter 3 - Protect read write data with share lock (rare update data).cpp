//#include <iostream>
//#include <mutex>
//#include <shared_mutex>
//#include <map>
//#include <string>
//using namespace std;
//
//class dns_entry{};
//class dns_cache {
//	map<string, dns_entry> entries;
//	//shared mute instead of normal mutex allow read/write separation of threads and allow better interaction
//	mutable shared_mutex entry_mutex;	//Mutable because mutex and mutable goes together (mutable allow separation
//										//of logical const and bitwise const. Logical const const-ness in interface
//									//Changing the state of the mutex doesn't change the class object --> logical
//public:
//	dns_entry find_entry (string const& domainName) const {	//Reader access - mutiple threads can run as same time
//		shared_lock guard{ entry_mutex };
//		map<string, dns_entry>::const_iterator it{ entries.find (domainName) };
//		return (it != entries.end () ? it->second : dns_entry ());	//If the entries is found then return the correct IP
//	}
//	void update_or_add_entry (string const& domainName, dns_entry const& dns_details) {
//		scoped_lock blockGuard{ entry_mutex };	//This will block all other read/shared lock when reach
//		entries[domainName] = dns_details;
//	}
//};