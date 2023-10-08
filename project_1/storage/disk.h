#ifndef DISK_H
#define DISK_H

#include "../structs.h"

#include <string>
#include <math.h>
#include <vector>
 
class Disk {
private:
    uchar* startAddress;
    size_t size;
    size_t blkSize;
    size_t recordSize;
    uint numUsedBlks;
    size_t curBlkUsedMem; // used memory in the current block

    bool allocateBlock();

public:
    Disk(size_t size, size_t blkSize, size_t recordSize);

    ~Disk();

    Record* writeRecord(Record record);

    void deleteRecord(Record* address);

    int getNumBlks() {return numUsedBlks;}

    int getBlockSize() {return blkSize;} 

    int getMaxRecordsPerBlock() {return floor(blkSize / recordSize);}

    int getBlockId(Record *record);

    Record *getRecord(int blockIdx, int recordIdx); 
};

#endif