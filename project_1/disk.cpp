#include "disk.h"

#include <cstdlib>
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


Record* Disk::writeRecord() {
    Record* address;
    if (!deletedRecords.empty()) {
        address = deletedRecords.back();
        deletedRecords.pop_back();
    }
    else {
        if (curBlkUsedMem + recordSize > blkSize) {
            if (!allocateBlock())
                return nullptr;
        }
        address = reinterpret_cast<Record*>(startAddress + numUsedBlks * blkSize + curBlkUsedMem);
        curBlkUsedMem += recordSize;
    }
    return address;
}

void Disk::deleteRecord(Record* address) {
    delete address;
    deletedRecords.push_back(address);
}



