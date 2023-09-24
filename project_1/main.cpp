#include "disk.h"
#include "b_plus_tree.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unordered_set>

using namespace std;

void experiment1(Disk *disk, BPTree *tree){
    cout << "Experiment 1:" << endl;
    //To change later after function is established
    int numRecords = readFileIntoDisk(disk, tree);
     cout << "Number of records: " << numRecords << endl;
    cout << "Size of a record: " << sizeof(Record) << " bytes" << endl;
    //To change later after function is established
    cout << "Number of records stored in a block: " << disk->getRecordsPerBlock() << endl;
    cout << "Number of blocks used: " << disk->getNumBlks() << endl;
    //To change later after function is established
    cout << "Total Memory Used: " << disk->getTotalUsedMemory() / pow(2, 20) << " MB" << endl;

}

void experiment2(BPTree *tree) {
    cout << "Experiment 2:" << endl;
    cout << "Instantiating B+ Tree" << endl;
    cout << " -> Nodes bounded by block size of = " << tree->getBlockSize() << endl;
    cout << " -> Maximum number of keys in a node: n = " << tree->getMaxKeys() << endl;
    cout << "Parameter N = " << tree->getMaxKeys() << endl;
    cout << "Number of nodes = " << tree->getTotalNumOfNodes() << endl;
    cout << "Number of levels = " << tree->getDepth() << endl;
    cout << "Content of Root Node: ";
    tree->displayKeys(tree->getRoot());
    cout << endl;
}

void experiment3(Disk *disk, BPTree *tree) {
    tree->setNumOfNodesAccessed(0);
    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    vector<Record *> *result = tree->searchRecord(0.5);
    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    chrono::duration<double> timeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    unordered_set<size_t> resultSet;
    float total_FG_PCT_home = 0;
    for (Record *r : *result) {
        resultSet.insert(disk->getBlockId(r));
        total_FG_PCT_home += r->fg3_pct_home;
    }
    total_FG_PCT_home /= result->size();

    int numOfBlocksAccessed = 0;
    Record *r;
    before = chrono::high_resolution_clock::now();
    for (int i = 0; i < disk->getBlocksUsed(); i++) {
        numOfBlocksAccessed++;
        for (int j = 0; j < disk->getRecordsPerBlock(); j++) {
            r = disk->getRecord(i, j);
            if (r->fg_pct_home == 0.5) {
                continue;
            }
        }
    }
    after = chrono::high_resolution_clock::now();
    chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    cout << "Experiment 3:" << endl;
    cout << "Number of index nodes accessed = " << tree->getNumOfNodesAccessed() << endl;
    cout << "Number of data blocks accessed = " << resultSet.size() << endl;
    cout << "Average FG_PCT_home = " << total_FG_PCT_home << endl;
    cout << "Running time for retrieval process = " << timeTaken.count() << "s" << endl;
    cout << "Number of data blocks accessed by brute force method = " << numOfBlocksAccessed << endl;
    cout << "Running time for retrieval by brute force method = " << bruteTimeTaken.count() << "s" << endl;
    cout << endl;
}

void experiment4(Disk *disk, BPTree *tree) {
    vector<Record *> result;
    vector<int> keys;
    float lower = 0.6;
    float upper = 1.0;
    int lowerIdx, upperIdx, leafNodesAccessed = 0;
    bool searching = true;
    tree->setNumOfNodesAccessed(0);

    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    Node *resultNode = tree->searchNode(lower);
    while (searching) {
        keys = resultNode->getKeys();
        lowerIdx = lower_bound(keys.begin(), keys.end(), lower) - keys.begin();
        upperIdx = lower_bound(keys.begin(), keys.end(), upper) - keys.begin();
        for (int i = lowerIdx; i < upperIdx; i++) {
            for (Record *r : resultNode->getRecords(i)) {
                result.push_back(r);
            }
        }
        upperIdx = upperIdx == keys.size() ? upperIdx - 1 : upperIdx;
        if (keys.at(upperIdx) >= upper) {
            searching = false;
        } else {
            resultNode = resultNode->getNxtLeaf();
            leafNodesAccessed++;
        }
    }
    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    chrono::duration<double> timeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    unordered_set<size_t> resultSet;
    float total_FG_PCT_home = 0;
    for (Record *r : result) {
        resultSet.insert(disk->getBlockId(r));
        total_FG_PCT_home += r->fg3_pct_home;
    }
    total_FG_PCT_home /= result.size();

    int numOfBlocksAccessed = 0;
    Record *r;
    before = chrono::high_resolution_clock::now();
    for (int i = 0; i < disk->getBlocksUsed(); i++) {
        numOfBlocksAccessed++;
        for (int j = 0; j < disk->getRecordsPerBlock(); j++) {
            r = disk->getRecord(i, j);
            if (r->fg_pct_home >= lower && r->fg_pct_home <= upper) {
                continue;
            }
        }
    }
    after = chrono::high_resolution_clock::now();
    chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    cout << "Experiment 4:" << endl;
    cout << "Number of index nodes accessed = " << tree->getNumOfNodesAccessed() + leafNodesAccessed << endl;
    cout << "Number of data blocks accessed = " << resultSet.size() << endl;
    cout << "Average FG3_PCT_home = " << total_FG_PCT_home << endl;
    cout << "Running time for retrieval process = " << timeTaken.count() << "s" << endl;
    cout << "Number of data blocks accessed by brute force method = " << numOfBlocksAccessed << endl;
    cout << "Running time for retrieval by brute force method = " << bruteTimeTaken.count() << "s" << endl;
    cout << endl;
}

void experiment5(Disk *disk, BpTree *tree) {
    float below_keyToDelete = 0.35;
    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    tree->deleteRecordsBelowThreshold(below_keyToDelete);
    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    chrono::duration<double> timeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    int numOfBlocksAccessed = 0;
    Record *r;
    before = chrono::high_resolution_clock::now();
    for (int i = 0; i < disk->getBlocksUsed(); i++) {
        numOfBlocksAccessed++;
        for (int j = 0; j < disk->getRecordsPerBlock(); j++) {
            r = disk->getRecord(i, j);
            if (r->fg_pct_home < below_keyToDelete) {
                continue;
            }
        }
    }
    after = chrono::high_resolution_clock::now();
    chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    cout << "Experiment 5:" << endl;
    cout << "Number of nodes of the updated B+ Tree = " << tree->getTotalNumOfNodes() << endl;
    cout << "Number of levels of the updated B+ Tree = " << tree->getDepth() << endl;
    cout << "Content of Root Node of updated B+ Tree: ";
    tree->displayKeys(tree->getRoot());
    cout << "Running time for deletion process = " << timeTaken.count() << "s" << endl;
    cout << "Number of data blocks accessed by brute force method = " << numOfBlocksAccessed << endl;
    cout << "Running time for deletion by brute force method = " << bruteTimeTaken.count() << "s" << endl;
    cout << endl;
}



int main() {
    Disk disk(500000, 400, sizeof(Record));
    cout << disk.writeRecord() << endl;
}