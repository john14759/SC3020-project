#include "b_plus_tree.h"
#include <algorithm>
// Adding these cause my compiler is shit
// #include "tree_search.cpp"
/////////////////////////////////////////
void BPTree::insert(float key, Record *recordPtr) {
    // duplicate key
    std::vector<Record *> *records = this->searchRecord(key);
    if (records != nullptr) {
        records->push_back(recordPtr);
        return;
    }

    // tree not instantiated
    if (this->root == nullptr) {
        this->root = new Node(true);
        this->numNodes++;
        this->depth++;
        this->root->keys.push_back(key);
        this->root->records.push_back(std::vector<Record *>(1, recordPtr));
        this->root->nextNodePtr = nullptr;
        return;
    }

    Node *currNode = this->root; 
    std::vector<Node *> parNodes(1, nullptr); 
    int idx = 0;

    // find leaf node to insert key
    while (!currNode->isLeaf) {
        idx = std::upper_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
        parNodes.push_back(currNode);
        currNode = currNode->ptrs.at(idx);
    }

    // insert key and record
    idx = std::upper_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
    currNode->keys.insert(currNode->keys.begin() + idx, key);
    currNode->records.insert(currNode->records.begin() + idx, std::vector<Record*>(1, recordPtr));

    // leaf node keys > max keys, split required
    if (currNode->keys.size() > this->maxKeys) {
        Node* newNode = this->splitLeafNode(currNode);
        Node* parNode = parNodes.back();
        parNodes.pop_back();
        key = newNode->keys.front();

        while (parNode != nullptr && parNode->keys.size() == this->maxKeys) {
            // keys in parent node > max keys, split required
            idx = std::upper_bound(parNode->keys.begin(), parNode->keys.end(), key) - parNode->keys.begin();
            parNode->keys.insert(parNode->keys.begin() + idx, key);
            parNode->ptrs.insert(parNode->ptrs.begin() + idx + 1, newNode);

            newNode = this->splitInternalNode(parNode, &key);
            currNode = parNode;

            parNode = parNodes.back();
            parNodes.pop_back();
        }  

        if (parNode == nullptr) {
            parNode = new Node(false);
            this->numNodes++;
            parNode->keys.push_back(key);
            parNode->ptrs.push_back(currNode);
            parNode->ptrs.push_back(newNode);
            this->root = parNode;
            this->depth++;
            return;
        } 
        else {
            // keys in parent node < max keys, no split required
            idx = std::upper_bound(parNode->keys.begin(), parNode->keys.end(), key) - parNode->keys.begin();
            parNode->keys.insert(parNode->keys.begin() + idx, key);
            parNode->ptrs.insert(parNode->ptrs.begin() + idx + 1, newNode);
        }
    }   
}

Node* BPTree::splitLeafNode(Node* currNode) {
    Node* splitNode = new Node(true);
    this->numNodes++;

    for (int i=0; i<(this->maxKeys+1)/2; i++) {
        splitNode->keys.insert(splitNode->keys.begin(), currNode->keys.back());
        currNode->keys.pop_back();
        splitNode->records.insert(splitNode->records.begin(), currNode->records.back());
        currNode->records.pop_back();
    }
    splitNode->nextNodePtr = currNode->nextNodePtr;
    currNode->nextNodePtr = splitNode;

    return splitNode;
}

Node* BPTree::splitInternalNode(Node* currNode, float *key) {
    Node* splitNode = new Node(false);
    this->numNodes++;

    for (int i=0; i < this->maxKeys / 2; i++) {
        splitNode->keys.insert(splitNode->keys.begin(), currNode->keys.back());
        currNode->keys.pop_back();
        splitNode->ptrs.insert(splitNode->ptrs.begin(), currNode->ptrs.back());
        currNode->ptrs.pop_back();
    }

    splitNode->ptrs.insert(splitNode->ptrs.begin(), currNode->ptrs.back());
    currNode->ptrs.pop_back();
    *key = currNode->keys.back();
    currNode->keys.pop_back();

    return splitNode;
}