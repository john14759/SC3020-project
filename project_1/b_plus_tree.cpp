#include "b_plus_tree.h"

BPTree::BPTree(int blkSize) {
    this->root = nullptr;
    this->blkSize = blkSize;
    this->maxKeys = (blkSize - sizeof(float*)) / (sizeof(float) + sizeof(float*));
    this->numNodes = 0;
    this->depth = 0;
}

BPTree::~BPTree() {}

