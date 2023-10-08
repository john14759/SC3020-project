#include "b_plus_tree.h"
#include "../structs.h"
#include <algorithm>

Node* BPTree::searchNode(float key) {
    Node* curNode = getRoot();
    // If the tree is empty, return nullptr
    if (curNode == nullptr)
        return nullptr;
    this->numNodesAcc++;
    int idx;
    // Continue traversing the tree until a leaf node is reached
    while (!curNode->isLeaf) {
        // Find the index where the key would be inserted in the current node's keys
        idx = std::upper_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
        curNode = curNode->ptrs.at(idx);
        this->numNodesAcc++;
    }
    return curNode;
}

std::vector<Record*>* BPTree::searchRecord(float key) {
    Node* curNode = getRoot();

    // If there is no root node, return nullptr
    if (curNode == nullptr)
        return nullptr;
    this->numNodesAcc++;
    int idx;
    
    // Traverse the tree until reaching a leaf node
    while (!curNode->isLeaf) {
        // Find the index of the next child node to traverse
        idx = std::upper_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
        curNode = curNode->ptrs.at(idx);
        this->numNodesAcc++;
    }
    
    // Find the index of the key in the leaf node
    idx = std::lower_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
    // If the key is found, return a pointer to the associated records
    if (idx < curNode->keys.size() && curNode->keys.at(idx) == key) 
        return &(curNode->records.at(idx));
    return nullptr;
}
