#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include "structs.h"

#include <vector>
 
class MemoryPool {
private:
    void *startAdd;
    uint size;
    usint blkSize;
    usint recordSize;
    uint numUsedBlks;
    usint curBlkUsedMem; // used memory in the current block
    std::vector<Record*> deletedRecords;

    bool allocateBlock();

public:
    MemoryPool(uint size, usint blkSize, usint recordSize);

    ~MemoryPool();

    Record* writeRecord();

    void deleteRecord(Record* address);

    int getNumBlks() {return numUsedBlks;}
};

#endif