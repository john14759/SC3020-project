#include "utils.h"
#include "lib/date.h"
#include "storage/disk.h"
#include "tree/b_plus_tree.h"
#include <unordered_set>


#include <iostream>

using namespace std;

void experiment1(Disk *disk, BPTree *tree){
    cout << "Experiment 1:" << endl;
    cout << "Number of records: " << utils::readFileIntoDisk("games.txt", disk, tree) << endl;
    cout << "Size of a record: " << sizeof(Record) << " bytes" << endl;
    cout << "Number of records stored in a block: " << disk->getBlkMaxRecords() << endl;
    cout << "Number of blocks used to store data in the disk: " << disk->getNumBlks() << endl;
}

void experiment2(BPTree *tree) {
    cout << "Experiment 2:" << endl;
    cout << "Printing B+ tree out:" << endl;
    cout << " " << endl;
    tree->printTree();
    cout << "Parameter n of the B+ tree = " << tree->getMaxKeys() << endl;
    cout << "Number of nodes of the B+ tree = " << tree->getNumNodes() << endl;
    cout << "Number of levels of the B+ tree = " << tree->getDepth() << endl;
    cout << "Content of Root Node of the B+ tree: ";
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
    unordered_set<int> resultSet;
    float total_FG_PCT_home = 0;
    for (Record *r : *result) {
        resultSet.insert(disk->getBlockId(r));
        total_FG_PCT_home += r->fg3_pct_home;
    }
    total_FG_PCT_home /= result->size();
    cout << "Experiment 3:" << endl;
    cout << "Number of index nodes accessed = " << tree->getNumNodesAcc() << endl;
    cout << "Number of data blocks accessed = " << resultSet.size() << endl;
    cout << "Average FG_PCT_home = " << total_FG_PCT_home << endl;
    cout << "Running time for retrieval process = " << timeTaken.count() << "s" << endl;
    cout << endl;

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

    cout << "Experiment 4:" << endl;
    cout << "Number of index nodes accessed = " << tree->getNumNodesAcc() + leafNodesAccessed << endl;
    cout << "Number of data blocks accessed = " << result.size() << endl;
    cout << "Average FG3_PCT_home = " << total_FG_PCT_home << endl;
    cout << "Running time for retrieval process = " << timeTaken.count() << "s" << endl;
    cout << endl;
}

void experiment5(Disk *disk, BPTree *tree) {
    /*
    std::vector<Record *> *result = tree->searchRecord(0.333);
    int recordsintree = 0;
    if ( result != nullptr){
        for (Record * recordptr : *result){
            recordsintree++;
            cout << "fg_pct_home: " << (*recordptr).fg_pct_home << endl;
            cout << recordsintree << endl;
        }
    }
    */
    float below_keyToDelete = 0.35;
    Node * root = tree->getRoot();
    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    tree->deleteRecordsBelowThreshold(root, below_keyToDelete);
    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    chrono::duration<double> timeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);

    cout << "Experiment 5:" << endl;
    cout << "Number of nodes of the updated B+ Tree = " << tree->getNumNodes() << endl;
    cout << "Number of levels of the updated B+ Tree = " << tree->getDepth() << endl;
    cout << "Content of Root Node of updated B+ Tree: " <<endl;
    tree->printNode(tree->getRoot());
    cout << "Running time for deletion process = " << timeTaken.count() << "s" << endl;
    cout << endl;
}

// Function to delete movies with "FG_PCT_home" below 0.35 inclusively for experiment 5
void deleteRecordsWithLowFGPCT(Disk* disk) {
    // Get the number of blocks in the disk
    size_t numBlocks = disk->getNumBlks();
    // Get the maximum number of records per block
    size_t maxRecordsPerBlock = disk->getBlkMaxRecords();
    // Get the size of a record in bytes
    size_t recordSize = sizeof(Record);
    // Initialize the count of blocks accessed to 0
    int numOfBlocksAccessed = 0;
    // Initialize the count of records deleted to 0
    int numofRecordsDeleted = 0;
    // Start the timer
    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    // Iterate over each block in the disk
    for (size_t blockIdx = 0; blockIdx < numBlocks; ++blockIdx) {
        // Increment the count of blocks accessed
        numOfBlocksAccessed++;
        // Iterate over each record in the block
        for (size_t recordIdx = 0; recordIdx < maxRecordsPerBlock; ++recordIdx) {
            // Get a pointer to the current record
            Record* record = disk->getRecord(blockIdx, recordIdx);
            
            // Check if the "FG_PCT_home" of the record is below or equal to 0.35
            if (record->fg_pct_home <= 0.35) {
                // Increment the count of records deleted
                numofRecordsDeleted++;
                // Continue to the next record
                continue;
            }
        }
    }
    // Stop the timer
    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    // Calculate the time taken for the deletion process
    chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);
    // Print the number of data blocks accessed
    cout << "Number of data blocks accessed by brute force method = " << numOfBlocksAccessed << endl;
    // Print the running time for the deletion process
    cout << "Running time for deletion by brute force method = " << bruteTimeTaken.count() << "s" << endl;
}

// Function to delete movies with "FG_PCT_home" 0.6 to 1 inclusively for experiment 4
void accessRecordsWithRange(Disk* disk) {
    // Get the number of blocks in the disk
    size_t numBlocks = disk->getNumBlks();
    // Get the maximum number of records per block
    size_t maxRecordsPerBlock = disk->getBlkMaxRecords();
    // Get the size of a single record
    size_t recordSize = sizeof(Record);
    // Initialize variables to keep track of the number of blocks and records accessed
    int numOfBlocksAccessed = 0;
    int numofRecordsAccessed = 0;
    // Start the timer
    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    
    // Loop through each block in the disk
    for (size_t blockIdx = 0; blockIdx < numBlocks; ++blockIdx) {
        // Increment the number of blocks accessed
        numOfBlocksAccessed++;
        
        // Loop through each record in the block
        for (size_t recordIdx = 0; recordIdx < maxRecordsPerBlock; ++recordIdx) {
            // Get the current record
            Record* record = disk->getRecord(blockIdx, recordIdx);
            
            // Check if the "FG_PCT_home" value of the record is between 0.6 and 1 (inclusive)
            if (0.6 <= record->fg_pct_home && record->fg_pct_home <= 1) {
                // Increment the number of records accessed
                numofRecordsAccessed++;
                // Continue to the next record
                continue;
            }
        }
    }
    // Stop the timer
    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    // Calculate the time taken by the brute force method
    chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);
    
    // Print the number of data blocks accessed
    cout << "Number of data blocks accessed by brute force method = " << numOfBlocksAccessed << endl;
    // Print the running time of the brute force method
    cout << "Running time for accessed by brute force method = " << bruteTimeTaken.count() << "s" << endl;
}

// Function to delete movies with "FG_PCT_home" equal to 0.5 for experiment 3
void accessRecordsWithEqual(Disk* disk) {
    // Get the number of blocks, maximum records per block, and record size from the disk
    size_t numBlocks = disk->getNumBlks();
    size_t maxRecordsPerBlock = disk->getBlkMaxRecords();
    size_t recordSize = sizeof(Record);
    // Variables to keep track of the number of blocks and records accessed
    int numOfBlocksAccessed = 0;
    int numofRecordsAccessed = 0;
    // Start measuring the execution time
    chrono::high_resolution_clock::time_point before = chrono::high_resolution_clock::now();
    // Loop through each block
    for (size_t blockIdx = 0; blockIdx < numBlocks; ++blockIdx) {
        // Increment the number of blocks accessed
        numOfBlocksAccessed++;
        // Loop through each record in the block
        for (size_t recordIdx = 0; recordIdx < maxRecordsPerBlock; ++recordIdx) {
            // Get the record from the disk
            Record* record = disk->getRecord(blockIdx, recordIdx);
            
            // Check if the "FG_PCT_home" value of the record is equal to 0.5
            if (record->fg_pct_home = 0.5) {
                // Increment the number of records accessed
                numofRecordsAccessed++;
                // Continue to the next record
                continue;
            }
        }
    }
    // Stop measuring the execution time
    chrono::high_resolution_clock::time_point after = chrono::high_resolution_clock::now();
    // Calculate the time taken for the brute force method
    chrono::duration<double> bruteTimeTaken = chrono::duration_cast<chrono::duration<double>>(after - before);
    // Print the number of data blocks accessed by the brute force method
    cout << "Number of data blocks accessed by brute force method = " << numOfBlocksAccessed << endl;
    // Print the running time for the brute force method
    cout << "Running time for accessed by brute force method = " << bruteTimeTaken.count() << "s" << endl;
}


int main() {
    Disk* disk = new Disk(500000000, 400, sizeof(Record));
    BPTree* tree = new BPTree(400);
    utils::readFileIntoDisk("games.txt", disk, tree);
    cout << endl;
    experiment1(disk, tree);
    cout << endl;
    experiment2(tree);
    cout << endl;
    experiment3(disk, tree);
    accessRecordsWithEqual(disk);
    cout << endl;
    experiment4(disk, tree);
    accessRecordsWithRange(disk);
    cout << endl;
    experiment5(disk, tree);
    deleteRecordsWithLowFGPCT(disk);
    cout << endl;
}



