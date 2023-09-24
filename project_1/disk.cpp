#include "disk.h"

#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

int Disk::readFileIntoDisk() {
    ifstream inputFile;

    inputFile.open("games.txt");

    if (inputFile.is_open()) {
        cout << "File is Open" << endl;
    } else {
        cout << "File failed to Open" << endl;
    }

    string line;
    int numOfRecords = 0;
    getline(inputFile, line);
    while (getline(inputFile, line)) {
        istringstream iss(line);
        string GAME_DATE_EST, TEAM_ID_home, PTS_home, FG_PCT_home, FT_PCT_home;
        string FG3_PCT_home, AST_home, REB_home, HOME_TEAM_WINS;
        getline(iss, GAME_DATE_EST, '\t');
        getline(iss, TEAM_ID_home, '\t');
        getline(iss, PTS_home, '\t');
        getline(iss, FG_PCT_home, '\t');
        getline(iss, FT_PCT_home, '\t');
        getline(iss, FG3_PCT_home, '\t');
        getline(iss, AST_home, '\t');
        getline(iss, REB_home, '\t');
        getline(iss, HOME_TEAM_WINS, '\t');
        // cout << GAME_DATE_EST << " | " << TEAM_ID_home << " | " << PTS_home << " | " << FG_PCT_home << " | ";
        // cout << FT_PCT_home << " | " << FG3_PCT_home << " | " << AST_home << " | " << REB_home << " | ";
        // cout << HOME_TEAM_WINS << " | " << endl;
        numOfRecords++;
        // if (numOfRecords == 10) {
        //     break;
        // }
        Record record = {
            stoul(GAME_DATE_EST), 
            stoul(TEAM_ID_home),
            static_cast<char>(stoi(PTS_home)),
            stof(FG_PCT_home),
            stof(FT_PCT_home),
            stof(FG3_PCT_home),
            static_cast<char>(stoi(AST_home)),
            static_cast<char>(stoi(REB_home)),
            HOME_TEAM_WINS == "1" ? true : false
        };
        writeRecord(record);
    }

    // struct Record {
    // uint game_date_est;
    // uint team_id_home;
    // char pts_home;
    // float fg_pct_home;
    // float ft_pct_home;
    // float fg3_pct_home;
    // char ast_home;
    // char reb_home;
    // bool home_team_wins;
    // };

    inputFile.close();

    return numOfRecords;
}

Disk::Disk(size_t size, size_t blkSize, size_t recordSize) {
    startAddress = (uchar*)malloc(size);
    this->size = size;
    this->blkSize = blkSize;
    this->recordSize = recordSize;
    numUsedBlks = 0;
    curBlkUsedMem = 0;
}

Disk::~Disk() {
    free(startAddress);
};

bool Disk::allocateBlock() {
    if (blkSize * (numUsedBlks + 1) > size) {
        cout << "Memory full" << endl;
        return false;
    }
    numUsedBlks++;
    curBlkUsedMem = 0;
    return true;
}


Record* Disk::writeRecord(Record record) {
    Record* recordAddress;

    // struct Record {
    // uint game_date_est;
    // uint team_id_home;
    // char pts_home;
    // float fg_pct_home;
    // float ft_pct_home;
    // float fg3_pct_home;
    // char ast_home;
    // char reb_home;
    // bool home_team_wins;
    // };
    if (!deletedRecords.empty()) {
        recordAddress = deletedRecords.back();
        deletedRecords.pop_back();
    }
    else {
        if (curBlkUsedMem + recordSize > blkSize) {
            if (!allocateBlock())
                return nullptr;
        }
        recordAddress = reinterpret_cast<Record*>(startAddress + numUsedBlks * blkSize + curBlkUsedMem);
        curBlkUsedMem += recordSize;
    }
    *recordAddress = record;
    cout << record.game_date_est << endl;
    cout << record.team_id_home << endl;
    cout << record.pts_home << endl;
    cout << record.fg_pct_home << endl;
    cout << record.ft_pct_home << endl;
    cout << record.fg3_pct_home << endl;
    cout << record.ast_home << endl;
    cout << record.reb_home << endl;
    cout << record.home_team_wins << endl;
    cout << endl;
    return recordAddress;
}

void Disk::deleteRecord(Record* address) {
    delete address;
    deletedRecords.push_back(address);
}



