#ifndef DISK_H
#define DISK_H

#include "../structs.h"

#include <string>
#include <vector>
 
class Disk {
private:
    uchar* startAddress;
    size_t size;
    size_t blkSize;
    size_t recordSize;
    uint numUsedBlks;
    size_t curBlkUsedMem; // used memory in the current block
    size_t maxRecordsperBlock;

    bool allocateBlock();

public:
    Disk(size_t size, size_t blkSize, size_t recordSize);

    ~Disk();

    Record* writeRecord(Record record);

    void deleteRecord(Record* address);

    int getNumBlks() {return numUsedBlks;}

    int getBlockSize(){return blkSize;} 

    int getmaxRecordsperBlock(){return maxRecordsperBlock;}

    size_t getBlockId(Record *record);

    Record *getRecord(size_t blockIdx, size_t recordIdx);
};

#endif