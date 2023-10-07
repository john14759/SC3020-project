#include "b_plus_tree.h"
#include <algorithm>
#include <iostream>

void BPTree::deleteKey(float key) {
    if (this->root == nullptr) {
        std::cout << "Empty tree" << std::endl;
        return;
    }

    std::vector<Record *> *r = this->searchRecord(key);
    // key not in tree
    if (r == nullptr) {
        std::cout << key << " does not exist in the tree" << std::endl;
        return;
    }

    Node *currNode = this->root;
    std::vector<Node *> parents(1, nullptr);
    std::vector<int> prevIdxs;
    int idx, minKeys = (this->maxKeys + 1) / 2;

    // find the leaf node containing key. 
    // save internal nodes visited and indexes used
    while (!currNode->isLeaf) {
        idx = std::upper_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
        parents.push_back(currNode);
        prevIdxs.push_back(idx);
        currNode = currNode->ptrs.at(idx);
    }

    // find key's index in leaf node then delete key and its records
    idx = std::lower_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
    currNode->keys.erase(currNode->keys.begin() + idx);
    currNode->records.erase(currNode->records.begin() + idx);

    if (currNode == this->root && currNode->keys.size() == 0) {
        // no more keys in the tree
        this->setRoot(nullptr);
    }

    // keys in leaf node > minimum num of keys
    if (currNode->keys.size() >= minKeys) { 
        // key not left most
        if (idx != 0)
            return;  

        while (parents.back() != nullptr)
        {
            // iteratively check parent
            if (prevIdxs.back() == 0) {
                parents.pop_back();
                prevIdxs.pop_back();
            }
            // update key with new left most key of leaf node
            else {
                key = currNode->keys.front();
                currNode = parents.back();
                idx = prevIdxs.back();
                currNode->keys[idx - 1] = key;
                return;
            }
        }
    }
    // keys in leaf node < min num of keys
    else {
        int parIdx = prevIdxs.back();
        prevIdxs.pop_back();
        Node *leftNeighbour = nullptr, *rightNeighbour = nullptr;
        Node *parNode = parents.back();
        parents.pop_back();

        // there is left neighbour from same parent node
        if (parIdx > 0) {
            leftNeighbour = parNode->ptrs.at(parIdx - 1);
            // borrow key from left neighbour
            if (leftNeighbour->keys.size() > minKeys) {
                currNode->keys.insert(currNode->keys.begin(), leftNeighbour->keys.back());
                currNode->records.insert(currNode->records.begin(), leftNeighbour->records.back());
                leftNeighbour->keys.pop_back();
                leftNeighbour->records.pop_back();
                parNode->keys[parIdx] = currNode->keys.front();
                return;
            }
        }
        // there is right neighbour from same parent node
        if (parIdx < parNode->keys.size() - 1) {
            rightNeighbour = parNode->ptrs.at(parIdx + 1);
            if (rightNeighbour->keys.size() > minKeys){
                // borrow key from right neighbour
                currNode->keys.insert(currNode->keys.end(), rightNeighbour->keys.front());
                currNode->records.insert(currNode->records.end(), rightNeighbour->records.front());
                rightNeighbour->keys.erase(rightNeighbour->keys.begin());
                rightNeighbour->records.erase(rightNeighbour->records.begin());
                parNode->keys[parIdx] = rightNeighbour->keys.front();

                if (!idx) 
                    this->updateParentKeys(currNode, parNode, parIdx, parents, prevIdxs);
                return;
            }
        }

        // left neighbour exists
        if (leftNeighbour != nullptr) {
            // left neighbour exists has min keys, merge
            while (currNode->keys.size() != 0) {
                leftNeighbour->keys.push_back(currNode->keys.front());
                leftNeighbour->records.push_back(currNode->records.front());
                currNode->keys.erase(currNode->keys.begin());
                currNode->records.erase(currNode->records.begin());
            }

            leftNeighbour->nextNodePtr = currNode->nextNodePtr;
            this->removeInternal(parNode->keys.at(parIdx - 2), parNode, currNode);
        }
        else {
            // right neighbour has min keys, merge
            while (rightNeighbour->keys.size() != 0) {
                currNode->keys.push_back(rightNeighbour->keys.front());
                currNode->records.push_back(rightNeighbour->records.front());
                rightNeighbour->keys.erase(rightNeighbour->keys.begin());
                rightNeighbour->records.erase(rightNeighbour->records.begin());
            }

            currNode->nextNodePtr = rightNeighbour->nextNodePtr;
            this->removeInternal(parNode->keys.at(parIdx), parNode, rightNeighbour);
        }
    }
}

Node *BPTree::findParentNode(Node *parNode, Node *childNode) {
    int key, idx;
    Node *currNode = childNode;
    while (!currNode->isLeaf)
        currNode = currNode->ptrs.front();
    key = currNode->keys.front();

    while (!parNode->isLeaf) {
        idx = std::upper_bound(parNode->keys.begin(), parNode->keys.end(), key) - parNode->keys.begin();
        if (parNode->ptrs.at(idx) == childNode)
            return parNode;
        else
            parNode = parNode->ptrs.at(idx);
    }
    return nullptr;
}

void BPTree::removeInternal(int key, Node *parNode, Node *nodeToDelete) {
    if (parNode == this->root) {
        if (parNode->keys.size() == 1) {
            if (parNode->ptrs.at(0) == nodeToDelete)
                this->setRoot(parNode->ptrs.at(1));
            else
                this->setRoot(parNode->ptrs.at(0));
            return;
        }
    }

    // delete nodeToDelete
    int idx = std::lower_bound(parNode->keys.begin(), parNode->keys.end(), key) - parNode->keys.begin();
    parNode->keys.erase(parNode->keys.begin() + idx);
    for (idx = 0; idx < parNode->ptrs.size(); idx++) {
        if (parNode->ptrs[idx] == nodeToDelete)
            break;
    }
    parNode->ptrs.erase(parNode->ptrs.begin() + idx);
    this->numNodes--;

    // parent node has sufficient keys
    if (parNode->keys.size() >= this->maxKeys / 2)
        return;

    // find parentNode's left and right neighbours
    Node *ancestorNode = this->findParentNode(this->root, parNode);
    for (idx = 0; idx < ancestorNode->ptrs.size(); idx++) {
        if (ancestorNode->ptrs.at(idx) == parNode)
            break;
    }
    Node *left, *right;
    if (idx > 0) {
        left = parNode->ptrs.at(idx - 1);
        if (left->keys.size() > this->maxKeys / 2) {
            parNode->keys.insert(parNode->keys.begin(), ancestorNode->keys.at(idx - 1));
            ancestorNode->keys[idx - 1] = left->keys.back();

            parNode->ptrs.insert(parNode->ptrs.begin(), left->ptrs.back());

            left->keys.pop_back();
            left->ptrs.pop_back();
        }
    }

    if (idx < ancestorNode->ptrs.size() - 1) {
        right = ancestorNode->ptrs.at(idx + 1);

        if (right->keys.size() > this->maxKeys / 2) {
            parNode->keys.push_back(ancestorNode->keys.at(idx));
            parNode->keys[idx] = right->keys.front();
            right->keys.erase(right->keys.begin());

            parNode->ptrs.push_back(right->ptrs.front());
            right->ptrs.erase(right->ptrs.begin());

            return;
        }
    }

    if (idx > 0) {
        left->keys.push_back(ancestorNode->keys.at(idx - 1));

        while (parNode->keys.size() != 0)
            left->keys.push_back(parNode->keys.front());
        
        while (parNode->ptrs.size() != 0)
            left->ptrs.push_back(parNode->ptrs.front());

        this->removeInternal(ancestorNode->keys.at(idx - 1), ancestorNode, parNode);
    }
    else if (idx < ancestorNode->ptrs.size() - 1) {
        parNode->keys.push_back(ancestorNode->keys.at(idx));

        while (right->keys.size() != 0)
            parNode->keys.push_back(right->keys.front());

        while (right->ptrs.size() != 0)
            parNode->ptrs.push_back(right->ptrs.front());

        this->removeInternal(ancestorNode->keys.at(idx), ancestorNode, right);
    }
}

void BPTree::updateParentKeys(Node *currNode, Node *parNode, int parIdx, std::vector<Node *> &parents, std::vector<int> &prevIdxs) {
    while (parNode != nullptr) {
        if (parIdx == 0) {
            parNode = parents.back();
            parents.pop_back();
            parIdx = prevIdxs.back();
            prevIdxs.pop_back();
        }
        else {
            int key = currNode->keys.front();
            parNode->keys[parIdx - 1] = key;
            break;
        }
    }
}

void BPTree::deleteRecordsBelowThreshold(Node* node, float threshold) {
    if (node == nullptr) 
        return;
    
    if (node->isLeaf) {
        for (int i = 0; i < node->keys.size(); i++) {
            if (node->keys[i] < threshold) {
                node->keys.erase(node->keys.begin() + i);
                node->records.erase(node->records.begin() + i);
                i--;  
            }
        }
    } 
    else {
        for (int i = 0; i < node->keys.size(); i++) {
            if (threshold < node->keys[i]) {
                deleteRecordsBelowThreshold(node->ptrs[i], threshold);
                return; 
            }
        }
        deleteRecordsBelowThreshold(node->ptrs.back(), threshold);
    }
}