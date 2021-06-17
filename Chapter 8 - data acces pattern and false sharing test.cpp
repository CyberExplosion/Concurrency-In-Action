#include <mutex>
#include <new>
using namespace std;

struct my_data {};

//* The best way for a thread to get the data is for the mutex and the data
//* to be close to each other. That mean when the thread load in to modify the
//* mutex, it will also load in the processor cache along with the mutex

/**
 *When another thread trying to access the mutex, it uses a read-modift-write atomic flag
 * operation. This operation could invalidated the cache line hold by the mutex owner thread
 * This is a performance hit, just by checking a mutex, the thread lost the cache line
 */
 //! The trick is to padded the mutex away from the data, so that when the cache line pick
//! up, it only pick up the data, and there will be no throwing back and forth data in cache

struct protected_data {
    mutex m;
    char padding[std::hardware_destructive_interference_size];
    my_data data_to_protect;
};

struct data_item {};

/**
 * False sharing happens when two unrelated data being so near together they in the same cache line
 * Each element of my_data is going to be accessed by different thread
 ** Therefore, it's better to padded them out so that each element doesn't stay on same cache line
*/
struct my_stuff {
    data_item d1;
    data_item d2;
    char padding[std::hardware_destructive_interference_size];  //* This padding will guarantee other element be on a different cache line
};
my_stuff some_array[256];    //! With the padding, each element is there own cache line

