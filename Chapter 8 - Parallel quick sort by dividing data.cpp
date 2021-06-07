#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <future>
using namespace std;

template <typename T>
class threadsafe_stack {};  //Assume we have one

template <typename T>
struct sorter {
    struct chunk_to_sort {
        list<T> data;
        promise<list<T>> promise;   //Pass data out
    };
    threadsafe_stack<chunk_to_sort> chunks; //List of data that other thread will work on when free
    vector<thread> threads; //No need to make it atomic since we going to use other method to check when thread pop
    unsigned const max_thread_count;
    atomic<bool> end_of_data;   //Flag to let thread to end their polling for new work

    sorter(): max_thread_count(thread::hardware_concurrency() - 1), end_of_data(false){}
        //Minus 1 because 1 thread is already the main thread
    ~sorter(){
        end_of_data = true;    //Signal other threads that sorting of every segments are done
        for (size_t i = 0; i < threads.size (); ++i) {
            threads.at (i).join ();
        }
    }

    //This function will pop a chunk from a stack for main thread to work on
    void try_sort_chunk () {
        shared_ptr<chunk_to_sort> chunk = chunks.pop ();    //Get a random work
        if (chunk) {
            sort_chunk (chunk);
        }
    }

    //Main recursive function to do quick sort
    /**
     ** Use another thread to take care of the lower half
     ** The higher half is going take care by main thread
    */
    list<T> do_sort (list<T>& chunk_data) {
        if (chunk_data.empty ()) {
            return chunk_data;
        }

        //* Main works, same thing as the parallel quick sorted before (the previous method use std::async, this time we create a thread pool)
        list<T> result;
        result.splice (result.begin (), chunk_data, chunk_data.begin ());
        T const& partition_val = *result.begin ();  //it's the pivot data //! The pivot is already in the result at this point
        typename list<T>::iterator divide_point = partition (chunk_data.begin (), chunk_data.end (),
            [&] (T const& val) {
                return val < partition_val; //Split into lower half (does not contain the pivot)
            });

        //! We basically has finish 1 iteration of the quick sort by this point (uses the pivot point and split into lower and higher half)
        //! Next step is to put our work onto the stack and spawn threads (if we can) to work on data on the stack

        chunk_to_sort new_lower_chunk;
            //*Put everything lower than the pivot point into the new lower chunk
        new_lower_chunk.data.splice (new_lower_chunk.data.end (), chunk_data, chunk_data.begin (), divide_point);
        future<list<T>> new_lower = new_lower_chunk.promise.get_future ();  //Connect so we can get result later
        chunks.push (std::move (new_lower_chunk));  //Put the lower half into the work stack for other threads

        if (threads.size () < max_thread_count) {   //Try spawning new thread to take care of the work
            threads.push_back (thread (&sorter<T>::sort_thread (), this));  //*Use this pointer when using a class member function
        }

        list<T> new_higher (do_sort (chunk_data));  //Spawn recursion to work on higher half
        result.splice (result.end (), new_higher);

        //! Now we free, we can start taking work off the stack
            //* While waiting for the result to return, help by using this thread to work on the data on the stack
        while (new_lower.wait_for (std::chrono::seconds (0)) != std::future_status::ready) {
            try_sort_chunk ();
        }
        result.splice (result.begin (), new_lower.get ());
        return result;
    }

    //Function sort chunk using main thread
    void sort_chunk (shared_ptr<chunk_to_sort> const& chunk) {
        chunk->promise.set_value (do_sort (chunk->data));   //ready data
    }

    //Sort chunk function to put in a new spawn thread
    void sort_thread () {
        while (!end_of_data) {
            try_sort_chunk ();
            this_thread::yield ();  //Give other thread a chance to do their try_sort_chunk too
        }
    }
};

template <typename T>
list<T> parallel_quick_sort(list<T> input){
    if (input.empty ()) {
        return input;
    }
    sorter<T> s;
    return s.do_sort (input);    //when the function return, it will activate the sorter destructor
//Lead to all threads join --> Make sure all works is finished by time the original function (not spawn by recursion)  return
}

int main () {
    list<int> wow;
    wow.push_back (5);
    wow.push_back (2);
    wow = parallel_quick_sort (wow);
}

/**
 * *This works best when we have a static data, not dynamically increase through time
 * * However, this is mainly not the case in real life application
 * * Therefore, it's better to divide threads by their jobs rather than the amount of job
 */