//#include <future>
//#include <map>
//#include <iostream>
//#include <array>
//using namespace std;
//
////std::async is the highest level future-provider for us. The other 2 are : packaged_task and promise
////async only allow us to execute ONE function at a time --> only 1 future
//	/*
//	If we have an array of futures ---> Ex: a lot of connections in an application
//		\
//		We need a lot of async --> However, that would also lead to a lot of threads. Since 1 async only spawn 1 threads, multiple call to async would be bad
//	*/
////Using promises, we can have ONE thread, providing MULTIPlE futures.
///*
//Ex: an arrya of multiple future would have array of multiple promises ==> in 1 thread. std::async would have trouble
//
//But in reality, maybe using packaged_task instead of promise would be Better than PLAIN promises
//*/
//
//
////You want to use Promises when a Simple RESULT from a function cannot neatly fit into std::async
//
//struct connection_set {};
//struct data_packet {
//	string payload;
//	int id;
//};
//struct outgoing_packet {
//	string payload;
//	int id;
//	promise<bool> promise;
//};
//map<int, promise<string>> promises_by_id;	//Imagine have a collection of promises in one thread, each with an id
//
//struct Connection {
//	promise<string>& get_promise (int data_id) {	//Promise just like thread cannot be copy
//		return promises_by_id.at (data_id);
//	}
//
//	data_packet incoming ();
//	outgoing_packet top_of_outgoing_queue ();
//	void send (string payload);
//
//	bool has_incoming_data ();
//	bool has_outgoing_data ();
//
//};
//
//bool done (connection_set);
//
////Start here
//		array<Connection,10> connections;		//Imagine there's a set of 10 connections
//void process_connection (connection_set& c) {
//	while (!done (c)) {
//		for (auto connection : connections) {	//Also if we call std::async on each connection, that would make each connection a thread
//												//A program can have up to 1000 connections, async going to make it 1000 threads -->Bad
//												//Have to use Promises -> the lowest level of Share-state multithreads
//			data_packet data = connection.incoming ();
//			if (connection.has_incoming_data ()) {	
//				promise<string>& p{ connection.get_promise (data.id) };	//get_promise is a custom function, it's not in std::promise
//				p.set_value (data.payload);	//Imagine the data payload is something very special and cannot fit good on async
//			}
//			if (connection.has_outgoing_data ()) {
//				outgoing_packet data{ connection.top_of_outgoing_queue () };
//				connection.send (data.payload);
//				data.promise.set_value (true);	//In the packet ther's a promise, use on OURSIDE to make sure the data it's send
//			}
//		}
//	}
//}