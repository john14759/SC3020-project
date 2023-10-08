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

void BPTree::setRoot(Node* r) {
    this->root = r;
}

void BPTree::printTree() {
    std::queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
        int n = q.size();
        for (int i = 0; i < n; i++) {
            Node* node = q.front();
            q.pop();
            printNode(node);
            // if internal node, push child nodes into queue
            if (!node->isLeaf) {
                for (Node* child : node->ptrs)
                    q.push(child);
            }
        }
        std::cout << std::endl;
    }
}

