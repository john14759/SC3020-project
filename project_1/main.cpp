#include "disk.h"

#include <iostream>

using namespace std;

int main() {
    Disk disk(500000, 400, sizeof(Record));
    cout << disk.writeRecord() << endl;
}