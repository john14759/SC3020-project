#include "memory_pool.h"

#include <cstdlib>
#include <iostream>

using namespace std;

MemoryPool::MemoryPool(uint size, usint blkSize, usint recordSize) {
    startAdd = malloc(size);
    this->size = size;
    this->blkSize = blkSize;
    this->recordSize = recordSize;
    numUsedBlks = 0;
    curBlkUsedMem = 0;
}

MemoryPool::~MemoryPool() {
    delete startAdd;
};

bool MemoryPool::allocateBlock() {
    if (blkSize * (numUsedBlks + 1) > size) {
        cout << "Memory full" << endl;
        return false;
    }
    numUsedBlks++;
    curBlkUsedMem = 0;
    return true;
}


Record* MemoryPool::writeRecord() {
    Record* address;
    if (!deletedRecords.empty()) {
        address = deletedRecords.back();
        deletedRecords.pop_back();
    }
    else {
        if (curBlkUsedMem + recordSize > blkSize) {
            if (!allocateBlock())
                return;
        }
        address = (Record*)startAdd + numUsedBlks * blkSize + curBlkUsedMem;
        curBlkUsedMem += recordSize;
    }
    return address;
}

void MemoryPool::deleteRecord(Record* address) {
    fill(address, address + recordSize, '\0');
    deletedRecords.push_back(address);
}



