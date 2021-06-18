#include <thread>
#include <numeric>
#include <vector>
#include <future>
using namespace std;

template<typename Iterator, typename T>
struct accumulate_block {
    T operator() (Iterator first, Iterator last) {      //Function object for easy usage
        return std::accumulate (first, last, T ());
    }
};

template<typename Iterator, typename T>
T parallel_accumulate (Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance (first, last);
    if (!length)
        return init;
    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
    /**
     * *This works because if use:
     *     max_theads = length / min_per_thread. We will round down the number and not get the correct thread count
     * *Ex: length = 7, min_per = 5 ---> max_thread = 1, even though we need 2 to work this
     * Therefore we would want to increase the length just enough to have the eqn produce another thread
     *  ==> + (min_per_thread - 1). Minus 1 to prevent the case if the length is divisible by min_per
     *  *Ex: length = 10, min_per = 5 ---> max_thread = 3, even though we only need 2, if we didn't use minus 1
     */
    unsigned long const hardware_threads = thread::hardware_concurrency ();
    unsigned long const num_threads = std::min (hardware_threads != 0 ? hardware_threads : 2, max_threads);
    //*Basically if cannot find hardware_thread, use 2. Then use the smaller number of threads from hardware or calculation as our number of working threads availabl
    unsigned long const block_size = length / num_threads;  //num_threads doesn't exclude this threads yet, we still just do basic calculation
    vector<future<T>> futures (num_threads - 1);    //Here we will minus out 1 thread, accounting for the thread we currently in
    vector<thread> threads (num_threads - 1);

    Iterator block_start = first;
    for (size_t i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance (block_end, block_size);
        packaged_task<T (Iterator, Iterator)> task (accumulate_block<Iterator, T> ());  //If it doesn't have name, (white color) it's a rvalue
        futures[i] = task.get_future ();    //Set the futures
        threads[i] = thread (std::move (task), block_start, block_end);
        block_start = block_end; //Move up to the next block
    }
    //* After the loop, we still have one thread with data left to do. THIS thread
    T last_result = accumulate_block<Iterator, T> ()(block_start, last);
    for_each (threads.begin (), threads.end (), [] (thread& th) {th.join (); });  //? Joining the threads seems tedious
    T result = init;
    for (T const& fut : futures) {
        result += fut.get ();
    }
    result += last_result;
    return result;
}
//* With packaged_task, the exception can propogate out when we use future.get()
//* ONly problem left is to deal with the thread that cannot join() because packaged_task throw()

//! The above is good, however if there's any exception get throw out due to either the
//! accumuate_block() fail, not all threads will be able to join if the function running
//! inside them throw(). We have to use .joinable() to check, and join() all other good threads

//? We can use try-catch block to test if thread is joinable(), but it's ugly and will introduce duplicate code
//Ex: we catch() and then join, even though if there's no exception we also have to join

//* Better option would be putting a wrapper around vector<thread>
/**
 * *it's the best way to clean up stuff in C++
 * *With the wrapper around the vector<thread>, we don't have to worry about manually joining
 * *the thread at the end of the parallel_accumulate, we can just put the work into the wrapper destructor
 */
class join_threads {
    vector<thread>& threads;    //Only hold reference to save resources (and there's really nothing fancy we want the class to do)
public:
    explicit join_threads (vector<thread>& threads_) : threads (threads_) {}
    ~join_threads () {
        for (thread& th : threads) {
            if (th.joinable ())
                th.join ();
        }
    }
};

//*Then your new fancy, shorter function
template<typename Iterator, typename T>
T parallel_accumulate (Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance (first, last);
    if (!length)
        return init;
    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = thread::hardware_concurrency ();
    unsigned long const num_threads = std::min (hardware_threads != 0 ? hardware_threads : 2, max_threads);
    //*Basically if cannot find hardware_thread, use 2. Then use the smaller number of threads from hardware or calculation as our number of working threads availabl
    unsigned long const block_size = length / num_threads;  //num_threads doesn't exclude this threads yet, we still just do basic calculation
    vector<future<T>> futures (num_threads - 1);    //Here we will minus out 1 thread, accounting for the thread we currently in
    vector<thread> threads (num_threads - 1);

    //! New stuff
    join_threads joiner (futures);

    Iterator block_start = first;
    for (size_t i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance (block_end, block_size);
        packaged_task<T (Iterator, Iterator)> task (accumulate_block<Iterator, T> ());  //If it doesn't have name, (white color) it's a rvalue
        futures[i] = task.get_future ();    //Set the futures
        threads[i] = thread (std::move (task), block_start, block_end);
        block_start = block_end; //Move up to the next block
    }
    //* After the loop, we still have one thread with data left to do. THIS thread
    T last_result = accumulate_block<Iterator, T> ()(block_start, last);

    ////for_each (threads.begin (), threads.end (), [] (thread& th) {th.join (); });
    //! We no longer have to use the above code, now the wrapper will automatically join our thread at the end

    T result = init;
    for (T const& fut : futures) {
        result += fut.get ();
    }
    result += last_result;
    return result;
} //! joiner would call destructor and join all threads here