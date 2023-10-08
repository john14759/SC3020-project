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
    // Print opening bracket
    cout << "[ ";
    // Print each key in the node
    for (float key : node->keys)
        cout << key << " ";
    // Print closing bracket and newline character
    cout << "]\n";
}

// Set the root of the BPTree to the specified node
void BPTree::setRoot(Node* r) {
    this->root = r;
}

void BPTree::printTree() {
    // Create a queue to store the nodes
    std::queue<Node*> q;
    
    // Push the root node into the queue
    q.push(root);
    
    // While the queue is not empty
    while (!q.empty()) {
        // Get the number of nodes at the current level
        int n = q.size();
        
        // Process each node at the current level
        for (int i = 0; i < n; i++) {
            // Get the front node from the queue
            Node* node = q.front();
            q.pop();
            
            // Print the node
            printNode(node);
            
            // If the node is not a leaf node
            if (!node->isLeaf) {
                // Push all the child nodes into the queue
                for (Node* child : node->ptrs)
                    q.push(child);
            }
        }
        
        // Move to the next level, print a new line
        std::cout << std::endl;
    }
}

