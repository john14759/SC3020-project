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
    int numOfNodesAcc;

public:
    BPTree(int blkSize);

    ~BPTree();

    Node* getRoot() {return root;}

    Node* searchNode(float key);

    std::vector<Record*>* searchRecord(float key);
    //returns a pointer to a vector containing Record pointers

};
/*
//For the last experiment
void BPTree::deleteRecordsBelowThreshold(float threshold) {
    // Start from the root node and traverse the tree
    deleteRecordsBelowThreshold(root, threshold);
}

// Recursive function to delete records below the threshold
void BpTree::deleteRecordsBelowThreshold(Node* node, float threshold) {
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
*/
#endif