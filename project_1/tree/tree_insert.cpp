#include "b_plus_tree.h"
#include <algorithm>

// Adding these cause my compiler is shit
#include "tree_search.cpp"
/////////////////////////////////////////


void BPTree::insert(float key, Record *recordPtr) {
    /**
     * Case 0: Duplicate insertion
     * Case 1: Tree not instantiated
     * Case 2: Leaf node keys < max keys
     * Case 3: Leaf node keys == max keys
     * Case 3a: Parent node < max keys
     * Case 3b: Parent node = max keys, split required
     *
     */

    // Case 0: Duplicate insertion
    std::vector<Record *> *records = this->searchRecord(key);
    if (records != nullptr) {
        records->push_back(recordPtr);
        return;
    }
    // Case 1: B+ tree not instantiated
    if (this->root == nullptr) {
            // Node* root;
            // int maxKeys;
            // int numNodes;
            // int depth;
            // size_t blkSize;
        this->root = new Node(true);
        this->numNodes++;
        this->depth++;
        this->root->keys.push_back(key);
        this->root->records.push_back(std::vector<Record *>(1, recordPtr));
        this->root->nextNodePtr = nullptr;
        return;
    }
    // For cases 2 to 3
    Node *currNode = this->root; // This node is used for traversing
    std::vector<Node *> parentNodes(1, nullptr); // Vector used to contain pointers to parent nodes
    int index = 0;

    // Find the leaf node where the key should be inserted
    while (!currNode->isLeaf) {
        index = std::upper_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
        parentNodes.push_back(currNode);
        currNode = currNode->ptrs.at(index);
    }

    // Insert the key and record into the leaf node at the sorted index
    // Case 2: Leaf node keys < max keys
    index = std::upper_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
    currNode->keys.insert(currNode->keys.begin() + index, key);
    currNode->records.insert(currNode->records.begin() + index, std::vector<Record*>(1, recordPtr));

    // Case 3: Leaf node keys == max keys
    if (currNode->keys.size() > this->maxKeys) {
        Node* newNode = this->splitLeafNode(currNode);
        Node* parentNode = parentNodes.back();
        parentNodes.pop_back();
        key = newNode->keys.front();

        while (parentNode != nullptr && parentNode->keys.size() == this->maxKeys) {
            // Iteratively check if parent is not NULL and has max children
            // Case 3b: Parent node = max keys, split required
            index = std::upper_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
            parentNode->keys.insert(parentNode->keys.begin() + index, key);
            parentNode->ptrs.insert(parentNode->ptrs.begin() + index + 1, newNode);

            newNode = this->splitInternalNode(parentNode, &key);
            currNode = parentNode;

            parentNode = parentNodes.back();
            parentNodes.pop_back();
        }  

        if (parentNode == nullptr) {
            // Root has been reached
            parentNode = new Node(false);
            this->numNodes++;
            parentNode->keys.push_back(key);
            parentNode->ptrs.push_back(currNode);
            parentNode->ptrs.push_back(newNode);
            this->root = parentNode;
            this->depth++;
            return;
        } else {
            // Case 3a: parent node < max keys
            index = std::upper_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
            parentNode->keys.insert(parentNode->keys.begin() + index, key);
            parentNode->ptrs.insert(parentNode->ptrs.begin() + index + 1, newNode);
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

    for (int i=0; i<this->maxKeys/2; i++) {
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