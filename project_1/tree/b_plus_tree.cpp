#include "b_plus_tree.h"

#include <iostream>
#include <vector>

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
    cout << "] ";
}

void BPTree::printTree() {
    vector<Node*> q;
    q.push_back(root);
    while (!q.empty()) {
        int n = q.size()
        for (int i = 0; i < n; i++) {
            Node* node = q.front();
            printNode(node);
            if (!node->isLeaf) {
                for (Node* child : node->ptrs)
                    q.push_back(child);
            }
        }
        cout << endl;
    }
}

