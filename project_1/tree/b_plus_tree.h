#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include "../structs.h"

class BPTree {
private:
    Node* root;
    int maxKeys;
    int numNodes;
    int depth;
    size_t blkSize;

    void printNode(Node* node);

public:
    BPTree(int blkSize);

    ~BPTree();

    Node* getRoot() {return root;}

<<<<<<< HEAD
    void setRoot(Node *node);
=======
    int getMaxKeys() {return maxKeys;}

    int getNumNodes() {return numNodes;}

    int getDepth() {return depth;}
>>>>>>> 4a4fbfea4a5d97b774cdb520c40f972f2861fb17

    Node* searchNode(float key);

    void insert(float key, Record *recordPtr);

    std::vector<Record*>* searchRecord(float key);
    //returns a pointer to a vector containing Record pointers

    void printTree();

    Node* splitLeafNode(Node* currNode);

    Node* splitInternalNode(Node* currNode, float* key);

<<<<<<< HEAD
    void deleteKey(float key);
    
    Node *findParentNode(Node *currNode, Node *childNode);
    
    void removeInternal(int key, Node *parentNode, Node *nodeToDelete);
    
    void updateParentKeys(Node *currNode, Node *parentNode, int parentIndex, std::vector<Node *> &parents, std::vector<int> &prevIndexs);
=======
>>>>>>> 4a4fbfea4a5d97b774cdb520c40f972f2861fb17
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