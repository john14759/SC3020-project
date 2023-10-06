#include "b_plus_tree.h"
#include "../structs.h"
#include <algorithm>

Node *BPTree::searchNode(float key) {
    Node *curNode = getRoot();
    if (curNode == nullptr)
        return nullptr;
    this->numOfNodesAcc++;
    int idx;
    while (!curNode->isLeaf) {
        idx = std::upper_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
        curNode = curNode->ptrs.at(idx);
        this->numOfNodesAcc++;
    }
    return curNode;
}

std::vector<Record *>* BPTree::searchRecord(float key) {
    Node *curNode = getRoot();
    if (curNode == nullptr)
        return nullptr;
    this->numOfNodesAcc++;
    int idx;
    while (!curNode->isLeaf) {
        idx = std::upper_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
        curNode = curNode->ptrs.at(idx);
        this->numOfNodesAcc++;
    }
    idx = std::lower_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
    if (idx < curNode->keys.size() && curNode->keys.at(idx) == key) {
        return &(curNode->records.at(idx));
        /*std::vector<Record*>* result = new std::vector<Record*>;
        result->push_back(curNode->records.at(idx));
        return result;*/
    }
    return nullptr;
}
