#include "b_plus_tree.h"
#include <algorithm>

void BPTree::insert(float key, Record *recordPtr) {
    // Check if the key already exists in the tree
    std::vector<Record *> *records = this->searchRecord(key);
    if (records != nullptr) {
        // If the key exists, add the record to the existing vector of records
        records->push_back(recordPtr);
        return;
    }
    // If the tree is empty, create a new root node
    if (this->root == nullptr) {
        this->root = new Node(true);
        this->numNodes++;
        this->depth++;
        this->root->keys.push_back(key);
        this->root->records.push_back(std::vector<Record *>(1, recordPtr));
        this->root->nextNodePtr = nullptr;
        return;
    }
    // Traverse the tree to find the leaf node where the key should be inserted
    Node *currNode = this->root; // Starting from the root node
    std::vector<Node *> parentNodes(1, nullptr); // Vector to store parent nodes
    int index = 0;
    // Find the leaf node where the key should be inserted
    while (!currNode->isLeaf) {
        // Find the index where the key should be inserted in the current node
        index = std::upper_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
        parentNodes.push_back(currNode); // Store the parent node
        currNode = currNode->ptrs.at(index); // Move to the next node
    }
    // Insert the key and record into the leaf node at the sorted index
    index = std::upper_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
    currNode->keys.insert(currNode->keys.begin() + index, key);
    currNode->records.insert(currNode->records.begin() + index, std::vector<Record *>(1, recordPtr));
    // If the leaf node has exceeded the maximum number of keys
    if (currNode->keys.size() > this->maxKeys) {
        // Split the leaf node into two nodes
        Node* newNode = this->splitLeafNode(currNode);
        Node* parentNode = parentNodes.back(); // Get the parent node
        parentNodes.pop_back(); // Remove the parent node from the vector
        key = newNode->keys.front(); // Get the key of the new node
        // Iterate until the parent node is not null and has reached the maximum number of keys
        while (parentNode != nullptr && parentNode->keys.size() == this->maxKeys) {
            // Insert the key and the new node into the parent node
            index = std::upper_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
            parentNode->keys.insert(parentNode->keys.begin() + index, key);
            parentNode->ptrs.insert(parentNode->ptrs.begin() + index + 1, newNode);
            // Split the internal node and update the current node and parent node
            newNode = this->splitInternalNode(parentNode, &key);
            currNode = parentNode;
            // Get the next parent node
            parentNode = parentNodes.back();
            parentNodes.pop_back();
        }  
        if (parentNode == nullptr) {
            // If the root node has been reached, create a new root node
            parentNode = new Node(false);
            this->numNodes++;
            parNode->keys.push_back(key);
            parNode->ptrs.push_back(currNode);
            parNode->ptrs.push_back(newNode);
            this->root = parNode;
            this->depth++;
            return;
        } else {
            // If the parent node is not full, insert the key and the new node into it
            index = std::upper_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
            parentNode->keys.insert(parentNode->keys.begin() + index, key);
            parentNode->ptrs.insert(parentNode->ptrs.begin() + index + 1, newNode);
        }
    }   
}

// This function splits a leaf node of a B+ tree into two nodes
Node* BPTree::splitLeafNode(Node* currNode) {
    // Create a new node to store the split keys and records
    Node* splitNode = new Node(true);
    this->numNodes++;
    // Move half of the keys and records from the current node to the split node
    for (int i=0; i<(this->maxKeys+1)/2; i++) {
        // Move the key from the back of the current node to the front of the split node
        splitNode->keys.insert(splitNode->keys.begin(), currNode->keys.back());
        currNode->keys.pop_back();
        // Move the record from the back of the current node to the front of the split node
        splitNode->records.insert(splitNode->records.begin(), currNode->records.back());
        currNode->records.pop_back();
    }
    // Update the next node pointer of the split node to the next node pointer of the current node
    splitNode->nextNodePtr = currNode->nextNodePtr;
    // Update the next node pointer of the current node to the split node
    currNode->nextNodePtr = splitNode;
    // Return the split node
    return splitNode;
}

// This function splits an internal node in a B+ tree
Node* BPTree::splitInternalNode(Node* currNode, float *key) {
    // Create a new node to hold the split keys and pointers
    Node* splitNode = new Node(false);
    this->numNodes++;
    // Move the second half of the keys and pointers from the current node to the split node
    for (int i=0; i<this->maxKeys/2; i++) {
        splitNode->keys.insert(splitNode->keys.begin(), currNode->keys.back());
        currNode->keys.pop_back();
        splitNode->ptrs.insert(splitNode->ptrs.begin(), currNode->ptrs.back());
        currNode->ptrs.pop_back();
    }
    // Move the last pointer from the current node to the split node
    splitNode->ptrs.insert(splitNode->ptrs.begin(), currNode->ptrs.back());
    currNode->ptrs.pop_back();
    // Update the key that separates the current node and the split node
    *key = currNode->keys.back();
    currNode->keys.pop_back();
    // Return the split node
    return splitNode;
}