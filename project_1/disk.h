#ifndef DISK_H
#define DISK_H

#include "structs.h"

#include <vector>
 
class Disk {
private:
    uchar* startAddress; //pointer to a block of memory representing the start address of the disk
    size_t size;//size of the disk memory
    size_t blkSize;//size of each block in the disk
    size_t recordSize;//size of a single record
    uint numUsedBlks;//number of blocks used in the disk
    size_t curBlkUsedMem; // used memory in the current block
    std::vector<Record*> deletedRecords; // addresses of free space available from deleted records

    bool allocateBlock();

public:
    Disk(size_t size, size_t blkSize, size_t recordSize);/*constructor for a Disk class object, which initializes a disk with
    the specified size, block size and record size*/

    ~Disk();//destructor of an instance of Disk

    Record* writeRecord();//a function that writes to the disk and returns a pointer to the written record

    void deleteRecord(Record* address);//a function that deletes the record from the disk

    int getNumBlks() {return numUsedBlks;}//public function to return the number of used blocks
};

#endif