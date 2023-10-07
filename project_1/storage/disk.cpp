#include "disk.h"

#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h>

using namespace std;

Disk::Disk(size_t size, size_t blkSize, size_t recordSize) {
    startAddress = (uchar*)malloc(size);
    this->size = size;
    this->blkSize = blkSize;
    this->recordSize = recordSize;
    numUsedBlks = 0;
    curBlkUsedMem = 0;

    maxRecordsperBlock = floor(blkSize / recordSize);
}

Disk::~Disk() {
    free(startAddress);
};

// This function checks if there is enough memory available to allocate a new block.
// If there is enough memory, it increments the number of used blocks and resets the current block's used memory to zero.
// If there is not enough memory, it prints an error message and returns false.
bool Disk::allocateBlock() {
    // Check if allocating a new block will exceed the total size of the disk.
    if (blkSize * (numUsedBlks + 1) > size) {
        // Print an error message indicating that the memory is full.
        cout << "Memory full" << endl;
        // Return false to indicate that the block allocation failed.
        return false;
    }
    
    // Increment the number of used blocks.
    numUsedBlks++;
    
    // Reset the current block's used memory to zero.
    curBlkUsedMem = 0;
    
    // Return true to indicate that the block allocation was successful.
    return true;
}

// This function writes a record to a disk block and returns the address of the record.
// If there is not enough space in the current block, a new block is allocated.
Record* Disk::writeRecord(Record record) {
    // Check if there is enough space in the current block for the record.
    if (curBlkUsedMem + recordSize > blkSize) {
        // If there is not enough space, allocate a new block.
        if (!allocateBlock())
            return nullptr;
    }
    // Calculate the address of the record within the block.
    Record* recordAddress = reinterpret_cast<Record*>(startAddress + numUsedBlks * blkSize + curBlkUsedMem);
    // Increment the number of used blocks if this is the first record being written.
    if (numUsedBlks == 0)
        numUsedBlks++;
    // Update the amount of used memory in the current block.
    curBlkUsedMem += recordSize;
    // Write the record to the calculated address.
    *recordAddress = record;
    // Return the address of the written record.
    return recordAddress;
}

void Disk::deleteRecord(Record* address) {
    delete address;
}
// added
// Function to calculate the block ID of a record in the disk storage system
size_t Disk::getBlockId(Record* record) {
    // Convert the record pointer to a pointer to unsigned char
    uchar* recordBytes = reinterpret_cast<uchar*>(record);
    // Calculate the offset of the record from the start address of the disk
    ptrdiff_t offset = recordBytes - startAddress;
    // Calculate the block ID by dividing the offset by the block size
    size_t blockId = offset / blkSize;
    // Return the block ID
    return blockId;
}

// Retrieve a record from the disk based on the block index and record index
Record* Disk::getRecord(size_t blockIdx, size_t recordIdx) {
    // Calculate the offset of the record within the disk
    size_t offset = blockIdx * blkSize + recordIdx * recordSize;
    
    // Interpret the memory at the calculated offset as a Record pointer
    return reinterpret_cast<Record *>(startAddress + offset);
}
