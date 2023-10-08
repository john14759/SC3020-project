#include "b_plus_tree.h"
#include <algorithm>
#include <iostream>
#include <stack>

void BPTree::deleteKey(float key) {
    // Check if the tree is empty
    if (this->root == nullptr) {
        std::cout << "Tree is empty" << std::endl;
        return;
    }
    // Search for the key in the tree
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
        Node *left = nullptr, *right = nullptr;
        Node *parNode = parents.back();
        parents.pop_back();

        // there is left neighbour from same parent node
        if (parIdx > 0) {
            left = parNode->ptrs.at(parIdx - 1);
            // borrow key from left neighbour
            if (left->keys.size() > minKeys) {
                currNode->keys.insert(currNode->keys.begin(), left->keys.back());
                currNode->records.insert(currNode->records.begin(), left->records.back());
                left->keys.pop_back();
                left->records.pop_back();
                parNode->keys[parIdx] = currNode->keys.front();
                return;
            }
        }
        // there is right neighbour from same parent node
        if (parIdx < parNode->keys.size() - 1) {
            right = parNode->ptrs.at(parIdx + 1);
            if (right->keys.size() > minKeys) {
                // borrow key from right neighbour
                currNode->keys.insert(currNode->keys.end(), right->keys.front());
                currNode->records.insert(currNode->records.end(), right->records.front());
                right->keys.erase(right->keys.begin());
                right->records.erase(right->records.begin());
                parNode->keys[parIdx] = right->keys.front();

                if (!idx) 
                    this->updateParentKeys(currNode, parNode, parIdx, parents, prevIdxs);
                return;
            }
        }

        // left neighbour exists
        if (left != nullptr) {
            // left neighbour exists has min keys, merge
            while (currNode->keys.size() != 0) {
                left->keys.push_back(currNode->keys.front());
                left->records.push_back(currNode->records.front());
                currNode->keys.erase(currNode->keys.begin());
                currNode->records.erase(currNode->records.begin());
            }

            left->nextNodePtr = currNode->nextNodePtr;
            this->removeInternal(parNode->keys.at(parIdx - 2), parNode, currNode);
        }
        else {
            // right neighbour has min keys, merge
            while (right->keys.size() != 0) {
                currNode->keys.push_back(right->keys.front());
                currNode->records.push_back(right->records.front());
                right->keys.erase(right->keys.begin());
                right->records.erase(right->records.begin());
            }

            currNode->nextNodePtr = right->nextNodePtr;
            this->removeInternal(parNode->keys.at(parIdx), parNode, right);
        }
    }
}

// This function finds the parent node of a given child node in a B+ tree.
Node* BPTree::findParentNode(Node *parNode, Node *childNode) {
    int key, idx;
    Node *currNode = childNode;
    // Traverse from the child node to the leaf node
    while (!currNode->isLeaf) {
        currNode = currNode->ptrs.front();
    }
    // Get the key from the leaf node
    key = currNode->keys.front();
    // Traverse from the parent node to the leaf node
    while (!parNode->isLeaf) {
        // Find the index where the key should be inserted in the parent node
        idx = std::upper_bound(parNode->keys.begin(), parNode->keys.end(), key) - parNode->keys.begin();
        // Check if the child node is found at the current index in the parent node
        if (parNode->ptrs.at(idx) == childNode) {
            return parNode;
        } else {
            // Update the parent node to the next level
            parNode = parNode->ptrs.at(idx);
        }
    }
    // If the parent node is not found, return nullptr
    return nullptr;
}

void BPTree::removeInternal(int key, Node *parNode, Node *nodeToDelete) {   
    // Check if the parent node is the root
    if (parNode == this->root) {
        // Check if the parent node has only one key
        if (parNode->keys.size() == 1) {   
            // Set the root to the appropriate child node
            if (parNode->ptrs.at(0) == nodeToDelete)
                this->setRoot(parNode->ptrs.at(1));    
            else
                this->setRoot(parNode->ptrs.at(0));
            return;
        }
    }

    // Delete the key and pointer from the parent node
    int idx = std::lower_bound(parNode->keys.begin(), parNode->keys.end(), key) - parNode->keys.begin();
    parNode->keys.erase(parNode->keys.begin() + idx);
    for (idx = 0; idx < parNode->ptrs.size(); idx++) {
        if (parNode->ptrs[idx] == nodeToDelete)
            break;
    }
    parNode->ptrs.erase(parNode->ptrs.begin() + idx);
    this->numNodes--;

    // Return if the parent node has more than or equal to the minimum number of keys
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
        // Check if the left neighbor has more than the minimum number of keys
        if (left->keys.size() > this->maxKeys / 2) {
             // Update the keys and pointers in the parent node and left neighbor
            parNode->keys.insert(parNode->keys.begin(), ancestorNode->keys.at(idx - 1));
            ancestorNode->keys[idx - 1] = left->keys.back();

            parNode->ptrs.insert(parNode->ptrs.begin(), left->ptrs.back());

            left->keys.pop_back();
            left->ptrs.pop_back();
        }
    }

    if (idx < ancestorNode->ptrs.size() - 1) {
        right = ancestorNode->ptrs.at(idx + 1);

        // Check if the right neighbor has more than the minimum number of keys
        if (right->keys.size() > this->maxKeys / 2) {
            // Update the keys and pointers in the parent node and right neighbor
            parNode->keys.push_back(ancestorNode->keys.at(idx));
            parNode->keys[idx] = right->keys.front();
            right->keys.erase(right->keys.begin());
            parNode->ptrs.push_back(right->ptrs.front());
            right->ptrs.erase(right->ptrs.begin());

            return;
        }
    }
    // Merge with the left neighbor if it exists
    if (idx > 0) {
        // Move all keys and pointers from the parent node to the left neighbor
        left->keys.push_back(ancestorNode->keys.at(idx - 1));

        while (parNode->keys.size() != 0)
            left->keys.push_back(parNode->keys.front());
        
        // Move all pointers from the parent node to the left neighbor
        while (parNode->ptrs.size() != 0)
            // Add the first pointer of the parent node to the ptrs vector of the left neighbor
            left->ptrs.push_back(parNode->ptrs.front());
        
        // Remove the key at index - 1 from the ancestor node
        this->removeInternal(ancestorNode->keys.at(idx - 1), ancestorNode, parNode);
    }
    else if (idx < ancestorNode->ptrs.size() - 1) {
    // Merge with the right neighbor if it exists

    // Add the key from the ancestor node at index to the keys vector of the parent node
    
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

void BPTree::deleteRecordsBelowThreshold(Node* root, float threshold) {
    if (root == nullptr) {
        return;
    }
    std::stack<Node*> nodeStack;
    nodeStack.push(root);
    while (!nodeStack.empty()) {
        Node* currentNode = nodeStack.top();
        nodeStack.pop();
        if (currentNode->isLeaf) {
            // Create vectors to store the deleted records
            std::vector<float> deletedKeys;
            std::vector<std::vector<Record*>> deletedRecords;
            // Delete records that meet the criteria and track them
            for (int i = 0; i < currentNode->keys.size(); i++) {
                if (currentNode->keys[i] < threshold) {
                    deletedKeys.push_back(currentNode->keys[i]);
                    deletedRecords.push_back(currentNode->records[i]);
                    // Remove the record from this leaf node
                    currentNode->keys.erase(currentNode->keys.begin() + i);
                    currentNode->records.erase(currentNode->records.begin() + i);
                    i--;  // Adjust the index after erasing
                }
            }
            // Print the deleted records
            // for (size_t i = 0; i < deletedKeys.size(); i++) {
            //     std::cout << "Deleted Record with Key for B+ tree: " << deletedKeys[i] << std::endl;
            // }
        } else {
            // Traverse internal nodes
            for (int i = 0; i < currentNode->keys.size(); i++) {
                // Check if the threshold is less than the current key
                if (threshold < currentNode->keys[i]) {
                    // Push the child node onto the stack for further processing
                    nodeStack.push(currentNode->ptrs[i]);
                }

            }
            // Push the rightmost child onto the stack for further processing
            nodeStack.push(currentNode->ptrs.back());
        }
    }
}

