#include "b_plus_tree.h"

#include <iostream>
#include <queue>

using namespace std;

BPTree::BPTree(int blkSize) {
    this->root = nullptr;
    this->blkSize = blkSize;
    this->maxKeys = (blkSize - sizeof(float*)) / (sizeof(float) + sizeof(float*));
    this->numNodes = 0;
    this->depth = 0;
}

BPTree::~BPTree() {}

void BPTree::printNode(Node* node) {
    cout << "[ ";
    for (float key : node->keys)
        cout << key << " ";
    cout << "]\n";
}

void BPTree::setRoot(Node* r){
    this->root = r;
    return;
}

void BPTree::printTree() {
    queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
        int n = q.size();
        for (int i = 0; i < n; i++) {
            Node* node = q.front();
            q.pop();
            printNode(node);
            if (!node->isLeaf) {
                for (Node* child : node->ptrs)
                    q.push(child);
            }
        }
        cout << endl;
    }
}


// Recursive function to delete records below the threshold
void BPTree::deleteRecordsBelowThreshold(Node* node, float threshold) {
    if (node == nullptr) {
        return;
    }

    if (node->isLeaf) {
        // Delete records that meet the criteria
        for (int i = 0; i < node->keys.size(); i++) {
            if (node->keys[i] < threshold) {
                // Remove the record from this leaf node
                node->keys.erase(node->keys.begin() + i);
                node->records.erase(node->records.begin() + i);
                i--;  // Adjust the index after erasing
            }
        }
    } else {
        // Traverse internal nodes and recursively delete records
        for (int i = 0; i < node->keys.size(); i++) {
            // Check if the threshold is less than the current key
            if (threshold < node->keys[i]) {
                // Recursively delete records in the child node
                deleteRecordsBelowThreshold(node->ptrs[i], threshold);
                return; // Exit the loop since B+ trees are sorted
            }
        }

        // If the threshold is greater than or equal to the last key, traverse the rightmost child
        deleteRecordsBelowThreshold(node->ptrs.back(), threshold);
    }
}

