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

Node *BPTree::findParentNode(Node *parentNode, Node *childNode)
{
    int key, index;
    Node *currNode = childNode;
    while (!currNode->isLeaf)
    {
        currNode = currNode->ptrs.front();
    }
    key = currNode->keys.front();

    while (!parentNode->isLeaf)
    {
        index = std::upper_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
        if (parentNode->ptrs.at(index) == childNode)
        {
            return parentNode;
        }
        else
        {
            parentNode = parentNode->ptrs.at(index);
        }
    }

    return nullptr;
}

void BPTree::removeInternal(int key, Node *parentNode, Node *nodeToDelete)
{
    if (parentNode == this->root)
    {
        if (parentNode->keys.size() == 1)
        {
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

    // Delete the nodeToDelete
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

    // Return if the parentNode has more than the min number of keys
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
        if (leftNeighbour->keys.size() > this->maxKeys / 2)
        {
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

        if (rightNeighbour->keys.size() > this->maxKeys / 2)
        {
            parentNode->keys.push_back(ancestorNode->keys.at(index));
            parentNode->keys[index] = rightNeighbour->keys.front();
            rightNeighbour->keys.erase(rightNeighbour->keys.begin());

            parentNode->ptrs.push_back(rightNeighbour->ptrs.front());
            rightNeighbour->ptrs.erase(rightNeighbour->ptrs.begin());

            return;
        }
    }

    if (index > 0)
    {
        leftNeighbour->keys.push_back(ancestorNode->keys.at(index - 1));

        while (parentNode->keys.size() != 0)
        {
            leftNeighbour->keys.push_back(parentNode->keys.front());
        }

        while (parentNode->ptrs.size() != 0)
        {
            leftNeighbour->ptrs.push_back(parentNode->ptrs.front());
        }

        this->removeInternal(ancestorNode->keys.at(index - 1), ancestorNode, parentNode);
    }
    else if (index < ancestorNode->ptrs.size() - 1)
    {
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