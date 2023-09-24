#ifndef DISK_H
#define DISK_H

#include "structs.h"

#include <vector>
 
class Disk {
private:
    uchar* startAddress;
    size_t size;
    size_t blkSize;
    size_t recordSize;
    uint numUsedBlks;
    size_t curBlkUsedMem; // used memory in the current block
    std::vector<Record*> deletedRecords; // addresses of free space available from deleted records

    bool allocateBlock();

public:

    Disk(size_t size, size_t blkSize, size_t recordSize);

    ~Disk();

    Record* writeRecord();

    void deleteRecord(Record* address);

    int getNumBlks() {return numUsedBlks;}

    int ReadFileIntoDisk();
};

#endif