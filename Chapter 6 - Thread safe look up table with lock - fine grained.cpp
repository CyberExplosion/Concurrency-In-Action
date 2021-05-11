//#include <list>
//#include <vector>
//#include <shared_mutex>
//#include <map>
//using namespace std;
//
//template <typename Key, typename Value, typename Hash = std::hash<Key> >	//Default for 
//						//hash function would be hash<Key>, if the user doesn't want to 
//						//create their own hash function
//class bucket_type {	//Linked list is going to be the bucket
//private:
//	typedef std::pair<Key, Value> bucket_value;	//typedef is used to create an alias
//								//Here bucket value is what connects in the list
//	typedef std::list<bucket_value> bucket_data;	//Each bucket going to have a list
//				/*
//				Essentially we making an opened ended hash table
//				*/
//	typedef typename bucket_data::iterator bucket_iterator;	//Alias for the list iterator
//	bucket_data data;
//	mutable std::shared_mutex m;	//Our protection region is going to be the bucket list
//
//	auto find_entry_for (Key const& key) const /*return const bucket_iterator*/ {
//		//Only return a const iterator at that one value in the list inside bucket
//		return std::find_if (data.begin (), data.end (), [&] (bucket_value const& item) {
//			return item.first == key;
//			});
//	}
//public:
//	//Look up and return the value
//	//A Read operation, so used shared mutex
//	Value value_for (Key const& key, Value const& default_value) const {	//Allow user
//			//to pass in a default value for checking purposes, if nothing found then it
//			//will be return
//		shared_lock lock{ m };
//		auto found_entry = find_entry_for (key);
//		return found_entry == data.end () ? default_value : found_entry->second;
//	}
//
//	//Put add and update together so don't have to worry about the trouble of dealing with 
//	//data races.  Just like how we put pop and top together in concurrent stack
//	void add_or_update_mapping (Key const& key, Value const& value) {
//		scoped_lock guard{ m };	//Read-modify function, so need to strong lock
//		auto found_entry = find_entry_for (key);
//		if (found_entry == data.end ()) {
//			data.push_back (bucket_value (key, value));
//		} else {
//			found_entry->second = value;
//		}
//	}
//
//	void remove_mapping (Key const& key) {
//		scoped_lock guard{ m };	//Dangerous cause deleting value
//		bucket_iterator const found_entry = find_entry_for (key);
//		if (found_entry != data.end ()) {
//			data.erase (found_entry);
//		}
//	}
//};
//
//
//template <typename Key, typename Value, typename Hash=std::hash<Key> >	//Default for 
//						//hash function would be hash<Key>, if the user doesn't want to 
//						//create their own hash function
//class threadsafe_lookup_table {
//private:
//	typedef std::pair<Key, Value> bucket_value;	//typedef is used to create an alias
//							//Here bucket value is what connects in the list
//	typedef std::list<bucket_value> bucket_data;	//Each bucket going to have a list
//				/*
//				Essentially we making an opened ended hash table
//				*/
//	typedef typename bucket_data::iterator bucket_iterator;	//Alias for the list iterator
//	typedef typename bucket_type<Key, Value, Hash> bucket_type_;	//alias for the bucket type
//
//	vector<unique_ptr<bucket_type_>> buckets;	//Have them hold uniq_ptr so don't have to
//											//init object when not need yet
//	Hash hasher;	//Custom hasher
//
//	bucket_type_& get_bucket (Key const& key) const {	//probe function in the hash table
//		size_t const bucket_index = hasher (key) % buckets.size ();	//Use size_t when
//				//dealing with anything like storage size.
//			//Also use the modulus so that the range never goes out
//		return *(buckets.at (bucket_index));
//	}
//
//public:
//	typedef Key key_type;
//	typedef Value mapped_type;
//	typedef Hash hash_type;
//	
//	threadsafe_lookup_table (size_t num_buckets = 19, Hash const& hasher_ = Hash ())	//Default size is 19
//		: buckets (num_buckets), hasher (hasher_) {
//		for (size_t i = 0; i < num_buckets; i++) {
//			buckets.at (i).reset (new bucket_type_);
//		}
//	}
//	threadsafe_lookup_table (threadsafe_lookup_table const&) = delete;
//	threadsafe_lookup_table& operator= (threadsafe_lookup_table const&) = delete;
//
//	//No need for the locking below since we already provided fine-grained
//	Value value_for (Key const key, Value const& default_value = Value ()) const {
//		return get_bucket (key).add_or_update_mapping (key, default_value);
//	}
//	void add_or_update_mapping (Key const& key, Value const& value) {
//		get_bucket (key).add_or_update_mapping (key, value);
//	}
//	void remove_mapping (Key const& key) {
//		get_bucket (key).remove_mapping (key);
//	}
//
//	//Quality of life functions
//
//	//Function that returns a snapshot of all data 'at this point' in the whole table
//	std::map<Key, Value> get_map () const {
//		vector<unique_lock<shared_mutex>> guards;
//		for (size_t i = 0; i < buckets.size(); i++) {
//			guards.push_back (unique_lock<shared_mutex> (buckets.at (i).mutex));	//Locks each mutex
//												//in each buckets.  (in same order too so no deadlock later on
//		}
//		map<Key, Value> res;
//		for (size_t i = 0; i < buckets.size (); ++i) {
//			for (bucket_iterator it = buckets.at (i).data.begin (); it != buckets.at (i).data.end (); ++it) {
//				res.insert (*it);
//			}
//		}
//		return res;
//	}
//};
//
//int main () {
//
//}