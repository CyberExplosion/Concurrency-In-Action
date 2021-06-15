#include <new>
#include <iostream>
using namespace std;

int main () {
    cout << "Space requires to not have false sharing: "
        << std::hardware_destructive_interference_size << endl;
}