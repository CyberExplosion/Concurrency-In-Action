//#include <iostream>
//using namespace std;
//
//int main () {
//	atomic<bool> b (true);	//Initialize-able, can't copy construct
//	b = false;	//Can assign to just like normal bool
//
//	bool x = b.load (std::memory_order_acquire);	//load() is a read operation
//		//Atomic only returns by value, never reference to prevent other thread
//		//modifying the result passing out.
//	b.store (true);	//store() is a write operation.
//	b = true;	//You can also just assign
//	x = b.exchange (false, std::memory_order_acq_rel);	//exchange is a 
//		//read-modify-write operation. It will RETURNS the value of "current" b. Then change it into the new value provides in the argument.
//
//	//Use compare_exchange as a compare-and-exchange operation
//	bool expected{ false };
//	extern atomic<bool> atom;	//This indicate that the value is init somewhere else
//	while (!b.compare_exchange_weak (expected, true) && !expected);
//	/*
//	Compare and exchange operation follow:
//		- If the atomic variable is equal to the value "expected". Then the atomic value will be assign the "2nd PARAMETER" value (this case b will be true). And the function will return TRUE
//		- If the atomic variable is different than the "expected. Then the "expected" value will be assigned with the "atmoic value" (This case "expected" will become b - and b could be either true or false).
//	With the way the compare-and-exchange work, it guarantee that if you run the operation twice without any thread meddling with the atomic value, the exchange function will return true (Since the expected value is assigned with the atomic value after first FAIL exchange).
//
//	We usually want to put the compare_exhange_weak() in a loop because even with the atomic value = expected, spurious result can happen because CPU couldn't handle the single operation atomically.
//		(The !expected is there to check that if the operation return false, the expected value is going to be assigned with new value. This is to check it the "atom" variable is already TRUE, then the loop won't have to happen twice to check)
//	*/
//
//	b.compare_exchange_weak (expected, true, std::memory_order_acq_rel, std::memory_order_acquire);	//After 2 parameter, the 3rd one is for the TRUE case - which will assign the atomic value with the 2nd params ==> This is a write operation --> acquire-release
//	//The 4th params is for the FALSE case - which will READ the atomic value into "expected" which is just a normal data.
//
//	//When declaring memory order, the False case must be the same as the TRue case (minus the release part)
//	b.compare_exchange_weak (expected, true, memory_order_acq_rel);
//	//If you don't specified for failure, it's gonna be the same as order for success.
//
//
//	//This operation is most useful for making decision atomically I GUESS
//}