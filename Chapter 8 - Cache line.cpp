#include <new>
#include <iostream>
using namespace std;

int main () {
    //! The number of consecutive bytes that maybe involve in false sharing
    //! If ensure that unrelated data are this much apart-- There will be no false sharing
    cout << "Space requires to not have false sharing: "
        << hardware_destructive_interference_size << endl;

    //! If you fit data within this number of bytes, it will reduce the number of cache misses
    hardware_constructive_interference_size;
}