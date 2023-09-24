#include "disk.h"

#include <cstdlib>
#include <iostream>

using namespace std;

//initializes an instance of the Disk class
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
    //allocates a new block on the disk when needed
    //if the total size of all the blocks in the disk is greater than the allocated disk size
    if (blkSize * (numUsedBlks + 1) > size) {
        cout << "Memory full" << endl;
        return false;
    }
    numUsedBlks++;
    curBlkUsedMem = 0;
    return true;
}


Record* Disk::writeRecord() {
    Record* address;//declares a pointer to a Record called address
    if (!deletedRecords.empty()) {
        address = deletedRecords.back();
        deletedRecords.pop_back();
    }
    else {
        //checks if the current block has sufficient space to accomodate another record
        if (curBlkUsedMem + recordSize > blkSize) {
            //if there is insufficient space in the current block to accomodate another record
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



