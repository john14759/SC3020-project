#include "b_plus_tree.h"
#include <algorithm>
#include <iostream>

void BPTree::deleteKey(float key)
{
    if (this->root == nullptr)
    {
        std::cout << "Empty tree" << std::endl;
        return;
    }

    std::vector<Record *> *r = this->searchRecord(key);
    if (r == nullptr)
    {
        // Case 0: Key does not exist in the tree
        std::cout << key << " does not exist in the tree" << std::endl;
        return;
    }

    Node *currNode = this->root;
    std::vector<Node *> parents(1, nullptr);
    std::vector<int> prevIdxs;
    int idx, minKeys = (this->maxKeys + 1) / 2;

    // Find the leaf node where the key belongs, saving the internal nodes visited and indexes used
    while (!currNode->isLeaf)
    {
        idx = std::upper_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
        parents.push_back(currNode);
        prevIdxs.push_back(idx);
        currNode = currNode->ptrs.at(idx);
    }

    // Find the key's index in the leaf node, and delete the key and its records
    idx = std::lower_bound(currNode->keys.begin(), currNode->keys.end(), key) - currNode->keys.begin();
    currNode->keys.erase(currNode->keys.begin() + idx);
    currNode->records.erase(currNode->records.begin() + idx);

    if (currNode == this->root && currNode->keys.size() == 0)
    {
        // Check if the there are no more keys in the tree, in which case set root to null
        this->setRoot(nullptr);
    }

    if (currNode->keys.size() >= minKeys)
    {
        // Case 1: Leaf node has > minimum num of keys
        if (idx != 0)
        {
            // Case 1a: key is not the left most key, just delete
            return;
        }

        while (parents.back() != nullptr)
        {
            // Case 1b: key is the left most in the leaf node
            if (prevIdxs.back() == 0)
            {
                // If parent's index was 0 also, iteratively check its parent
                parents.pop_back();
                prevIdxs.pop_back();
            }
            else
            {
                // If parent's index was not 0, update the key with the new left most key of the leaf node
                key = currNode->keys.front();
                currNode = parents.back();
                idx = prevIdxs.back();
                currNode->keys[idx - 1] = key;
                return;
            }
        }
    }
    else
    {
        // Case 2: Leaf node now has less than min num of keys
        int parIdx = prevIdxs.back();
        prevIdxs.pop_back();
        Node *leftNeighbour = nullptr, *rightNeighbour = nullptr;
        Node *parNode = parents.back();
        parents.pop_back();

        if (parIdx > 0)
        {
            // Left neighbour from the same parent node exists
            leftNeighbour = parNode->ptrs.at(parIdx - 1);
            if (leftNeighbour->keys.size() > minKeys)
            {
                // Case 2a: Borrow key from left neighbour
                currNode->keys.insert(currNode->keys.begin(), leftNeighbour->keys.back());
                currNode->records.insert(currNode->records.begin(), leftNeighbour->records.back());
                leftNeighbour->keys.pop_back();
                leftNeighbour->records.pop_back();
                parNode->keys[parIdx] = currNode->keys.front();
                return;
            }
        }
        if (parIdx < parNode->keys.size() - 1)
        {
            // Right neighbour from the same parent node exists
            rightNeighbour = parNode->ptrs.at(parIdx + 1);
            if (rightNeighbour->keys.size() > minKeys)
            {
                // Case 2a: Borrow key from right neighbour
                currNode->keys.insert(currNode->keys.end(), rightNeighbour->keys.front());
                currNode->records.insert(currNode->records.end(), rightNeighbour->records.front());
                rightNeighbour->keys.erase(rightNeighbour->keys.begin());
                rightNeighbour->records.erase(rightNeighbour->records.begin());
                parNode->keys[parIdx] = rightNeighbour->keys.front();

                if (!idx)
                {
                    this->updateParentKeys(currNode, parNode, parIdx, parents, prevIdxs);
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
            this->removeInternal(parNode->keys.at(parIdx - 2), parNode, currNode);
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
            this->removeInternal(parNode->keys.at(parIdx), parNode, rightNeighbour);
        }
    }
}

Node *BPTree::findParentNode(Node *parNode, Node *childNode)
{
    int key, idx;
    Node *currNode = childNode;
    while (!currNode->isLeaf)
    {
        currNode = currNode->ptrs.front();
    }
    key = currNode->keys.front();

    while (!parNode->isLeaf)
    {
        idx = std::upper_bound(parNode->keys.begin(), parNode->keys.end(), key) - parNode->keys.begin();
        if (parNode->ptrs.at(idx) == childNode)
        {
            return parNode;
        }
        else
        {
            parNode = parNode->ptrs.at(idx);
        }
    }

    return nullptr;
}

void BPTree::removeInternal(int key, Node *parNode, Node *nodeToDelete)
{
    if (parNode == this->root)
    {
        if (parNode->keys.size() == 1)
        {
            if (parNode->ptrs.at(0) == nodeToDelete)
            {
                this->setRoot(parNode->ptrs.at(1));
            }
            else
            {
                this->setRoot(parNode->ptrs.at(0));
            }
            return;
        }
    }

    // Delete the nodeToDelete
    int idx = std::lower_bound(parNode->keys.begin(), parNode->keys.end(), key) - parNode->keys.begin();
    parNode->keys.erase(parNode->keys.begin() + idx);
    for (idx = 0; idx < parNode->ptrs.size(); idx++)
    {
        if (parNode->ptrs[idx] == nodeToDelete)
        {
            break;
        }
    }
    parNode->ptrs.erase(parNode->ptrs.begin() + idx);
    this->numNodes--;

    // Return if the parentNode has more than the min number of keys
    if (parNode->keys.size() >= this->maxKeys / 2)
        return;

    // Find the parentNode's left and right neighbours
    Node *ancestorNode = this->findParentNode(this->root, parNode);
    for (idx = 0; idx < ancestorNode->ptrs.size(); idx++)
    {
        if (ancestorNode->ptrs.at(idx) == parNode)
            break;
        
    }
    Node *leftNeighbour, *rightNeighbour;
    if (idx > 0)
    {
        leftNeighbour = parNode->ptrs.at(idx - 1);
        if (leftNeighbour->keys.size() > this->maxKeys / 2)
        {
            parNode->keys.insert(parNode->keys.begin(), ancestorNode->keys.at(idx - 1));
            ancestorNode->keys[idx - 1] = leftNeighbour->keys.back();

            parNode->ptrs.insert(parNode->ptrs.begin(), leftNeighbour->ptrs.back());

            leftNeighbour->keys.pop_back();
            leftNeighbour->ptrs.pop_back();
        }
    }

    if (idx < ancestorNode->ptrs.size() - 1)
    {
        rightNeighbour = ancestorNode->ptrs.at(idx + 1);

        if (rightNeighbour->keys.size() > this->maxKeys / 2)
        {
            parNode->keys.push_back(ancestorNode->keys.at(idx));
            parNode->keys[idx] = rightNeighbour->keys.front();
            rightNeighbour->keys.erase(rightNeighbour->keys.begin());

            parNode->ptrs.push_back(rightNeighbour->ptrs.front());
            rightNeighbour->ptrs.erase(rightNeighbour->ptrs.begin());

            return;
        }
    }

    if (idx > 0)
    {
        leftNeighbour->keys.push_back(ancestorNode->keys.at(idx - 1));

        while (parNode->keys.size() != 0)
        {
            leftNeighbour->keys.push_back(parNode->keys.front());
        }

        while (parNode->ptrs.size() != 0)
        {
            leftNeighbour->ptrs.push_back(parNode->ptrs.front());
        }

        this->removeInternal(ancestorNode->keys.at(idx - 1), ancestorNode, parNode);
    }
    else if (idx < ancestorNode->ptrs.size() - 1)
    {
        parNode->keys.push_back(ancestorNode->keys.at(idx));

        while (rightNeighbour->keys.size() != 0)
        {
            parNode->keys.push_back(rightNeighbour->keys.front());
        }

        while (rightNeighbour->ptrs.size() != 0)
        {
            parNode->ptrs.push_back(rightNeighbour->ptrs.front());
        }

        this->removeInternal(ancestorNode->keys.at(idx), ancestorNode, rightNeighbour);
    }
}

void BPTree::updateParentKeys(Node *currNode, Node *parNode, int parIdx, std::vector<Node *> &parents, std::vector<int> &prevIdxs)
{
    while (parNode != nullptr)
    {
        // Iteratively check and update the parent nodes
        if (parIdx == 0)
        {
            // If parent's index was 0 also, iteratively check its parent
            parNode = parents.back();
            parents.pop_back();
            parIdx = prevIdxs.back();
            prevIdxs.pop_back();
        }
        else
        {
            // If parent's index was not 0, update the key with the new left most key of the leaf node
            int key = currNode->keys.front();
            parNode->keys[parIdx - 1] = key;
            break;
        }
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