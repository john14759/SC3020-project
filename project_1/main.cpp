#include "memory_pool.h"

#include <iostream>

using namespace std;

int main() {
    MemoryPool disk(500000, 400, sizeof(Record));
    cout << disk.writeRecord() << endl;
}