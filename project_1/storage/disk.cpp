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

int Disk::getBlockId(Record* record){
    //this function takes a pointer to a record and returns a size_t value which represents the block ID of the 
    //record in the disk storage system
    uchar* recordBytes = reinterpret_cast<uchar*>(record);
    ptrdiff_t offset = recordBytes - startAddress;
    size_t blockId = offset / blkSize;
    return blockId;
}

Record* Disk::getRecord(int blockIdx, int recordIdx){
    //in this function, we use the block and record index, as well as block size and record size to locate the 
    //corresponding record
    size_t offset = blockIdx * blkSize + recordIdx * recordSize;
    return reinterpret_cast<Record *>(startAddress + offset);
}
