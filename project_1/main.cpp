#include "utils.h"
#include "lib/date.h"
#include "storage/disk.h"
#include "tree/b_plus_tree.h"
#include <unordered_set>

// Adding these cause my compiler is shit
// #include "tree/b_plus_tree.cpp"
/////////////////////////////////////////

#include <iostream>

using namespace std;

void experiment1(Disk *disk, BPTree *tree){
    cout << "Experiment 1:" << endl;
    cout << "Number of records: " << utils::readFileIntoDisk("games.txt", disk, tree) << endl;
    cout << "Size of a record: " << sizeof(Record) << " bytes" << endl;
    cout << "Number of records stored in a block: " << disk->getmaxRecordsperBlock() << endl;
    cout << "Number of blocks used: " << disk->getNumBlks() << endl;
}


void experiment2(BPTree *tree) {
    cout << "Experiment 2:" << endl;
    cout << "Printing B+ tree out:" << endl;
    cout << " " << endl;
    tree->printTree();
    //cout << " -> Nodes bounded by block size of = " << tree->getBlockSize() << endl;
    //cout << " -> Maximum number of keys in a node: n = " << tree->getMaxKeys() << endl;
    cout << "Parameter n of the B+ tree = " << tree->getMaxKeys() << endl;
    cout << "Number of nodes = " << tree->getNumNodes() << endl;
    cout << "Number of levels = " << tree->getDepth() << endl;
    cout << "Content of Root Node: ";
    tree->printNode(tree->getRoot());
    cout << endl;
}


void experiment3(Disk *disk, BPTree *tree) {
    tree->resetNumNodesAcc();
    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    vector<Record *> *result = tree->searchRecord(0.5);
    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    chrono::duration<double> timeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    //Calculation of the average FG3_PCT_HOME 
    unordered_set<size_t> resultSet;
    float total_FG_PCT_home = 0;
    for (Record *r : *result) {
        resultSet.insert(disk->getBlockId(r));
        total_FG_PCT_home += r->fg3_pct_home;
    }
    total_FG_PCT_home /= result->size();

    //Brute-force method of accessing the data from the disk directly
    int numOfBlocksAccessed = 0;
    Record *r;
    before = chrono::high_resolution_clock::now();
    //iterates through each block on the disk
    for (int i = 0; i < disk->getNumBlks(); i++) {
        numOfBlocksAccessed++;
        //iterates through each record on the block
        for (int j = 0; j < disk->getmaxRecordsperBlock(); j++) {
            r = disk->getRecord(i, j);
            if (r->fg_pct_home == 0.5) {
                continue;
            }
        }
    }
    after = chrono::high_resolution_clock::now();
    chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    cout << "Experiment 3:" << endl;
    cout << "Number of index nodes accessed = " << tree->getNumNodesAcc() << endl;
    cout << "Number of data blocks accessed = " << resultSet.size() << endl;
    cout << "Average FG_PCT_home = " << total_FG_PCT_home << endl;
    cout << "Running time for retrieval process = " << timeTaken.count() << "s" << endl;
    cout << "Number of data blocks accessed by brute force method = " << numOfBlocksAccessed << endl;
    cout << "Running time for retrieval by brute force method = " << bruteTimeTaken.count() << "s" << endl;
    cout << endl;

    // free(result);
    // free(&resultSet);
}

void experiment4(Disk *disk, BPTree *tree) {
    vector<Record *> result;
    vector<float> keys;
    float lower = 0.6;
    float upper = 1.0;
    int lowerIdx, upperIdx, leafNodesAccessed = 0;
    bool searching = true;
    tree->resetNumNodesAcc();

    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    Node *resultNode = tree->searchNode(lower);
    while (searching) {
        keys = resultNode->keys;
        lowerIdx = lower_bound(keys.begin(), keys.end(), lower) - keys.begin();
        upperIdx = lower_bound(keys.begin(), keys.end(), upper) - keys.begin();
        // cout << "lowerIdx: " << lowerIdx << endl;
        // cout << "upperIdx: " << upperIdx << endl;
        // cout << "Node record size: " << resultNode->records.size() << endl;
        // cout << "Node key size: " << resultNode->keys.size() << endl;
        // cout << "number of duplicate keys: " << resultNode->records[lowerIdx].size() << endl;
        // cout << "value at upper index: " << resultNode->records[upperIdx-1][0]->fg_pct_home << endl;
        for (int i=lowerIdx; i<=upperIdx-1; i++) {
            for (int j=0; j < resultNode->records[i].size(); j++) {
                  result.push_back(resultNode->records[i][j]);
                // cout << "value: " << resultNode->records[i][j]->fg_pct_home << endl;
            }
            // cout << "-----------------------------------------------" << i << endl;
        }
        upperIdx = upperIdx == keys.size() ? upperIdx - 1 : upperIdx;
        // cout << "keys.at(upperIdx): " << keys.at(upperIdx) << " " << "upper: " << upper << endl;
        if (keys.at(upperIdx) >= upper) {
            searching = false;
            // cout << "set Searching as false!" << endl;
        } else {
            resultNode = resultNode->nextNodePtr;
            if (resultNode == nullptr) {
                break;
            }
            leafNodesAccessed++;
        }
        // cout << "testing for segment error" << endl;
    }
    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    chrono::duration<double> timeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    // unordered_set<size_t> resultSet;
    float total_FG_PCT_home = 0;
    // for (Record *r : result) {
    //     resultSet.insert(disk->getBlockId(r));
    //     total_FG_PCT_home += r->fg3_pct_home;
    // }

    for (int i=0; i<result.size(); i++) {
        total_FG_PCT_home = total_FG_PCT_home + result[i]->fg3_pct_home;
        // cout << "fg3_pct_home: " << result[i]->fg3_pct_home << endl;
    }
    total_FG_PCT_home /= result.size();

    // // Brute force method below
    int numOfBlocksAccessed = 0;
    Record *r;
    // get the first leaf node of the tree first
    Node * currNode = tree->getRoot();
    while (currNode->isLeaf != true) {
        currNode = currNode->ptrs[0];
    }
    // currnode now contains the first leaf node
    // now need to iterate through the leaf nodes
    int i=0;
    // if (currNode->isLeaf) {
    //     cout << "is leaf node" << endl;
    // }
    // before = chrono::high_resolution_clock::now();
    // while (currNode->nextNodePtr != nullptr) {
    //     // Check if reached end of the leaf node
    //     if (i == currNode->keys.size()) {
    //         currNode = currNode->nextNodePtr;
    //         numOfBlocksAccessed += i;
    //         i = 0;
    //     }
    //     // Scan until hit first fg_pct_home > upper
    //     // if (currNode->records[i][0]->fg_pct_home >= upper) {
    //     //     break;
    //     // } 
    //     // cout << "i count: " << i << " keys.size(): " << currNode->keys.size() << endl;
    //     // cout << endl;
    //     i++;
    // }
    // // cout << "test" << endl;
    // after = chrono::high_resolution_clock::now();
    // chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    before = chrono::high_resolution_clock::now();
    for (int i = 0; i < disk->getNumBlks(); i++) {
        numOfBlocksAccessed++;
        for (int j = 0; j < disk->getmaxRecordsperBlock(); j++) {
            r = disk->getRecord(i, j);
            if (r->fg_pct_home >= lower && r->fg_pct_home <= upper) {
                continue;
            }
        }
    }
    after = chrono::high_resolution_clock::now();
    chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    cout << "Experiment 4:" << endl;
    cout << "Number of index nodes accessed = " << tree->getNumNodesAcc() + leafNodesAccessed << endl;
    cout << "Number of data blocks accessed = " << result.size() << endl;
    cout << "Average FG3_PCT_home = " << total_FG_PCT_home << endl;
    cout << "Running time for retrieval process = " << timeTaken.count() << "s" << endl;
    cout << "Number of data blocks accessed by brute force method = " << numOfBlocksAccessed << endl;
    cout << "Running time for retrieval by brute force method = " << bruteTimeTaken.count() << "s" << endl;
    cout << endl;
}

void experiment5(Disk *disk, BPTree *tree) {
    cout << "Number of nodes of the B+ Tree before deleting = " << tree->getNumNodes() << endl;
    float below_keyToDelete = 0.35;
    Node * root = tree->getRoot();
    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    tree->deleteRecordsBelowThreshold(root, below_keyToDelete);
    // delete records until reach first record with fg_pct_home > 0.35
    // get the first leaf node of the tree first
    // Node * currNode = tree->getRoot();
    // while (currNode->isLeaf != true) {
    //     currNode = currNode->ptrs[0];
    // }

    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    chrono::duration<double> timeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    int numOfBlocksAccessed = 1;
    Record *r;
    before = chrono::high_resolution_clock::now();
    // for (int i = 0; i < disk->getBlocksUsed(); i++) {
    //     numOfBlocksAccessed++;
    //     for (int j = 0; j < disk->getRecordsPerBlock(); j++) {
    //         r = disk->getRecord(i, j);
    //         if (r->fg_pct_home < below_keyToDelete) {
    //             continue;
    //         }
    //     }
    // }
    // get the first leaf node of the tree first
    Node * currNode = tree->getRoot();
    while (currNode->isLeaf != true) {
        currNode = currNode->ptrs[0];
    }
    // currNode now contains the first leaf node
    // Now need to iterate through every leaf node
    int i=0;
    while (currNode->nextNodePtr != nullptr) {
        // Check if reached end of the leaf node
        if (i == currNode->keys.size()) {
            currNode = currNode->nextNodePtr;
            numOfBlocksAccessed += i;
            i = 0;
        }
        // Scan until hit first fg_pct_home > upper
        // if (currNode->records[i][0]->fg_pct_home >= upper) {
        //     break;
        // } 
        // cout << "i count: " << i << " keys.size(): " << currNode->keys.size() << endl;
        i++;
    }
    after = chrono::high_resolution_clock::now();
    chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    cout << "Experiment 5:" << endl;
    cout << "Number of nodes of the updated B+ Tree = " << tree->getNumNodes() << endl;
    cout << "Number of levels of the updated B+ Tree = " << tree->getDepth() << endl;
    cout << "Content of Root Node of updated B+ Tree: ";
    // tree->displayKeys(tree->getRoot());
    cout << "Running time for deletion process = " << timeTaken.count() << "s" << endl;
    cout << "Number of data blocks accessed by brute force method = " << numOfBlocksAccessed << endl;
    cout << "Running time for deletion by brute force method = " << bruteTimeTaken.count() << "s" << endl;
    cout << endl;
}

int main() {
    Disk* disk = new Disk(500000000, 400, sizeof(Record));
    BPTree* tree = new BPTree(400);
    //cout << utils::readFileIntoDisk("games.txt", disk, tree) << endl;
    experiment1(disk, tree);
    cout << endl;
    experiment2(tree);
    cout << endl;
    experiment3(disk, tree);
    cout << endl;
    experiment4(disk, tree);
    cout << endl;
    experiment4(disk, tree);
    experiment5(disk, tree);
    //cout << tree->getMaxKeys() << endl;
    //cout << tree->getNumNodes() << endl;
    //cout << tree->getDepth() << endl;
}



