#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include "structs.h"

class BPTree {
private:
    Node* root;
    int maxKeys;
    int numNodes;
    int depth;
    size_t blkSize;

public:
    BPTree(int blkSize);

    ~BPTree();

    Node* getRoot() {return root;}

};

#endif