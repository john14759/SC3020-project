#include "disk.h"

#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

Disk::Disk(size_t size, size_t blkSize, size_t recordSize) {
    startAddress = (uchar*)malloc(size);
    this->size = size;
    this->blkSize = blkSize;
    this->recordSize = recordSize;
    numUsedBlks = 0;
    curBlkUsedMem = 0;
}

Disk::~Disk() {
    free(startAddress);
};

bool Disk::allocateBlock() {
    if (blkSize * (numUsedBlks + 1) > size) {
        cout << "Memory full" << endl;
        return false;
    }
    numUsedBlks++;
    curBlkUsedMem = 0;
    return true;
}

Record* Disk::writeRecord(Record record) {
    if (curBlkUsedMem + recordSize > blkSize) {
        if (!allocateBlock())
            return nullptr;
    }
    Record* recordAddress = reinterpret_cast<Record*>(startAddress + numUsedBlks * blkSize + curBlkUsedMem);
    if (numUsedBlks == 0)
        numUsedBlks++;
    curBlkUsedMem += recordSize;
    *recordAddress = record;
    return recordAddress;
}

void Disk::deleteRecord(Record* address) {
    delete address;
}



