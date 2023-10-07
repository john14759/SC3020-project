#include "b_plus_tree.h"
#include <algorithm>
#include <iostream>
#include <stack>

void BPTree::deleteKey(float key)
{
    // Check if the tree is empty
    if (this->root == nullptr)
    {
        std::cout << "Tree is empty" << std::endl;
        return;
    }
    // Search for the key in the tree
    std::vector<Record *> *r = this->searchRecord(key);
    if (r == nullptr)
    {
        // Case 0: Key does not exist in the tree
        std::cout << key << " does not exist in the tree" << std::endl;
        return;
    }

    Node *currNode = this->root;
    std::vector<Node *> parents(1, nullptr);
    std::vector<int> prevIndexs;
    short index, minKeys = (this->maxKeys + 1) / 2;

    // Find the leaf node where the key belongs, saving the internal nodes visited and indexes used
    while (!currNode->isLeaf)
    {
        index = std::upper_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
        parents.push_back(currNode);
        prevIndexs.push_back(index);
        currNode = currNode->ptrs.at(index);
    }

    // Find the key's index in the leaf node, and delete the key and its records
    index = std::lower_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
    currNode->keys.erase(currNode->keys.begin() + index);
    currNode->records.erase(currNode->records.begin() + index);

    if (currNode == this->root && currNode->keys.size() == 0)
    {
        // Check if the there are no more keys in the tree, in which case set root to null
        this->setRoot(nullptr);
    }

    if (currNode->keys.size() >= minKeys)
    {
        // Case 1: Leaf node has > minimum num of keys
        if (index != 0)
        {
            // Case 1a: key is not the left most key, just delete
            return;
        }

        while (parents.back() != nullptr)
        {
            // Case 1b: key is the left most in the leaf node
            if (prevIndexs.back() == 0)
            {
                // If parent's index was 0 also, iteratively check its parent
                parents.pop_back();
                prevIndexs.pop_back();
            }
            else
            {
                // If parent's index was not 0, update the key with the new left most key of the leaf node
                key = currNode->keys.front();
                currNode = parents.back();
                index = prevIndexs.back();
                currNode->keys[index - 1] = key;
                return;
            }
        }
    }
    else
    {
        // Case 2: Leaf node now has less than min num of keys
        int parentIndex = prevIndexs.back();
        prevIndexs.pop_back();
        Node *leftNeighbour = nullptr, *rightNeighbour = nullptr;
        Node *parentNode = parents.back();
        parents.pop_back();

        if (parentIndex > 0)
        {
            // Left neighbour from the same parent node exists
            leftNeighbour = parentNode->ptrs.at(parentIndex - 1);
            if (leftNeighbour->keys.size() > minKeys)
            {
                // Case 2a: Borrow key from left neighbour
                currNode->keys.insert(currNode->keys.begin(), leftNeighbour->keys.back());
                currNode->records.insert(currNode->records.begin(), leftNeighbour->records.back());
                leftNeighbour->keys.pop_back();
                leftNeighbour->records.pop_back();
                parentNode->keys[parentIndex] = currNode->keys.front();
                return;
            }
        }
        if (parentIndex < parentNode->keys.size() - 1)
        {
            // Right neighbour from the same parent node exists
            rightNeighbour = parentNode->ptrs.at(parentIndex + 1);
            if (rightNeighbour->keys.size() > minKeys)
            {
                // Case 2a: Borrow key from right neighbour
                currNode->keys.insert(currNode->keys.end(), rightNeighbour->keys.front());
                currNode->records.insert(currNode->records.end(), rightNeighbour->records.front());
                rightNeighbour->keys.erase(rightNeighbour->keys.begin());
                rightNeighbour->records.erase(rightNeighbour->records.begin());
                parentNode->keys[parentIndex] = rightNeighbour->keys.front();

                if (!index)
                {
                    this->updateParentKeys(currNode, parentNode, parentIndex, parents, prevIndexs);
                }
                return;
            }
        }

        // Check if left neighbour exists, if it does merge with it, else merge with right neighbour
        if (leftNeighbour != nullptr)
        {
            // Left neighbour exists but has min keys -> MERGE
            while (currNode->keys.size() != 0)
            {
                leftNeighbour->keys.push_back(currNode->keys.front());
                leftNeighbour->records.push_back(currNode->records.front());
                currNode->keys.erase(currNode->keys.begin());
                currNode->records.erase(currNode->records.begin());
            }

            leftNeighbour->nextNodePtr = currNode->nextNodePtr;
            this->removeInternal(parentNode->keys.at(parentIndex - 2), parentNode, currNode);
        }
        else
        {
            // Right neighbour exists but has min keys -> MERGE
            while (rightNeighbour->keys.size() != 0)
            {
                currNode->keys.push_back(rightNeighbour->keys.front());
                currNode->records.push_back(rightNeighbour->records.front());
                rightNeighbour->keys.erase(rightNeighbour->keys.begin());
                rightNeighbour->records.erase(rightNeighbour->records.begin());
            }

            currNode->nextNodePtr = rightNeighbour->nextNodePtr;
            this->removeInternal(parentNode->keys.at(parentIndex), parentNode, rightNeighbour);
        }
    }
}

// This function finds the parent node of a given child node in a B+ tree.
Node* BPTree::findParentNode(Node *parentNode, Node *childNode) {
    int key, index;
    Node *currNode = childNode;
    // Traverse from the child node to the leaf node
    while (!currNode->isLeaf) {
        currNode = currNode->ptrs.front();
    }
    // Get the key from the leaf node
    key = currNode->keys.front();
    // Traverse from the parent node to the leaf node
    while (!parentNode->isLeaf) {
        // Find the index where the key should be inserted in the parent node
        index = std::upper_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
        // Check if the child node is found at the current index in the parent node
        if (parentNode->ptrs.at(index) == childNode) {
            return parentNode;
        } else {
            // Update the parent node to the next level
            parentNode = parentNode->ptrs.at(index);
        }
    }
    // If the parent node is not found, return nullptr
    return nullptr;
}

void BPTree::removeInternal(int key, Node *parentNode, Node *nodeToDelete)
{   
    // Check if the parent node is the root
    if (parentNode == this->root)
    {
        // Check if the parent node has only one key
        if (parentNode->keys.size() == 1)
        {   
            // Set the root to the appropriate child node
            if (parentNode->ptrs.at(0) == nodeToDelete)
            {
                this->setRoot(parentNode->ptrs.at(1));
            }
            else
            {
                this->setRoot(parentNode->ptrs.at(0));
            }
            return;
        }
    }

    // Delete the key and pointer from the parent node
    int index = std::lower_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
    parentNode->keys.erase(parentNode->keys.begin() + index);
    for (index = 0; index < parentNode->ptrs.size(); index++)
    {
        if (parentNode->ptrs[index] == nodeToDelete)
        {
            break;
        }
    }
    parentNode->ptrs.erase(parentNode->ptrs.begin() + index);
    this->numNodes--;

    // Return if the parent node has more than or equal to the minimum number of keys
    if (parentNode->keys.size() >= this->maxKeys / 2)
    {
        return;
    }

    // Find the parentNode's left and right neighbours
    Node *ancestorNode = this->findParentNode(this->root, parentNode);
    for (index = 0; index < ancestorNode->ptrs.size(); index++)
    {
        if (ancestorNode->ptrs.at(index) == parentNode)
        {
            break;
        }
    }
    Node *leftNeighbour, *rightNeighbour;
    if (index > 0)
    {
        leftNeighbour = parentNode->ptrs.at(index - 1);
        // Check if the left neighbor has more than the minimum number of keys
        if (leftNeighbour->keys.size() > this->maxKeys / 2)
        {
             // Update the keys and pointers in the parent node and left neighbor
            parentNode->keys.insert(parentNode->keys.begin(), ancestorNode->keys.at(index - 1));
            ancestorNode->keys[index - 1] = leftNeighbour->keys.back();

            parentNode->ptrs.insert(parentNode->ptrs.begin(), leftNeighbour->ptrs.back());

            leftNeighbour->keys.pop_back();
            leftNeighbour->ptrs.pop_back();
        }
    }

    if (index < ancestorNode->ptrs.size() - 1)
    {
        rightNeighbour = ancestorNode->ptrs.at(index + 1);

        // Check if the right neighbor has more than the minimum number of keys
        if (rightNeighbour->keys.size() > this->maxKeys / 2)
        {
            // Update the keys and pointers in the parent node and right neighbor
            parentNode->keys.push_back(ancestorNode->keys.at(index));
            parentNode->keys[index] = rightNeighbour->keys.front();
            rightNeighbour->keys.erase(rightNeighbour->keys.begin());

            parentNode->ptrs.push_back(rightNeighbour->ptrs.front());
            rightNeighbour->ptrs.erase(rightNeighbour->ptrs.begin());

            return;
        }
    }
    // Merge with the left neighbor if it exists
    if (index > 0)
    {
        // Move all keys and pointers from the parent node to the left neighbor
        leftNeighbour->keys.push_back(ancestorNode->keys.at(index - 1));

        while (parentNode->keys.size() != 0)
        {
            leftNeighbour->keys.push_back(parentNode->keys.front());
        }
        // Move all pointers from the parent node to the left neighbor
        while (parentNode->ptrs.size() != 0)
        {
            // Add the first pointer of the parent node to the ptrs vector of the left neighbor
            leftNeighbour->ptrs.push_back(parentNode->ptrs.front());
        }
        // Remove the key at index - 1 from the ancestor node
        this->removeInternal(ancestorNode->keys.at(index - 1), ancestorNode, parentNode);
    }
    else if (index < ancestorNode->ptrs.size() - 1)
    {
    // Merge with the right neighbor if it exists

    // Add the key from the ancestor node at index to the keys vector of the parent node
    
        parentNode->keys.push_back(ancestorNode->keys.at(index));

        while (rightNeighbour->keys.size() != 0)
        {
            parentNode->keys.push_back(rightNeighbour->keys.front());
        }

        while (rightNeighbour->ptrs.size() != 0)
        {
            parentNode->ptrs.push_back(rightNeighbour->ptrs.front());
        }

        this->removeInternal(ancestorNode->keys.at(index), ancestorNode, rightNeighbour);
    }
}

void BPTree::updateParentKeys(Node *currNode, Node *parentNode, int parentIndex, std::vector<Node *> &parents, std::vector<int> &prevIndexs)
{
    while (parentNode != nullptr)
    {
        // Iteratively check and update the parent nodes
        if (parentIndex == 0)
        {
            // If parent's index was 0 also, iteratively check its parent
            parentNode = parents.back();
            parents.pop_back();
            parentIndex = prevIndexs.back();
            prevIndexs.pop_back();
        }
        else
        {
            // If parent's index was not 0, update the key with the new left most key of the leaf node
            int key = currNode->keys.front();
            parentNode->keys[parentIndex - 1] = key;
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

