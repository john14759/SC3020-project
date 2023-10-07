#ifndef UTILS_H
#define UTILS_H

#include "lib/date.h"
#include "storage/disk.h"
#include "tree/b_plus_tree.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
// Including these cause my compiler is shit
// #include "storage/disk.cpp"
// #include "tree/tree_insert.cpp"
/////////////////////////////////////////////

using namespace std;

namespace utils {
    int dateStringToDaysSinceEpoch(std::string date) {
        istringstream dateStream(date);
        int day, month, year;
        char delimiter;
        dateStream >> day >> delimiter >> month >> delimiter >> year;
        date::year_month_day ymd{date::year(year), date::month(month), date::day(day)};
        return date::sys_days{ymd}.time_since_epoch().count();
    }

    int readFileIntoDisk(std::string fileName, Disk* disk, BPTree* tree) {
        ifstream inputFile;
        inputFile.open(fileName);
        if (!inputFile.is_open()) {
            std::cout << "File failed to Open" << std::endl;
            return 0;
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

            // Added a condition to check for relevant Records
            if (PTS_home == "") 
                continue;          
            Record record = {
                stof(FG_PCT_home),
                stof(FT_PCT_home),
                stof(FG3_PCT_home),
                stoul(TEAM_ID_home),
                usint(dateStringToDaysSinceEpoch(GAME_DATE_EST)),
                static_cast<char>(stoi(PTS_home)),
                static_cast<char>(stoi(AST_home)),
                static_cast<char>(stoi(REB_home)),
                HOME_TEAM_WINS == "1" ? true : false
            };
            Record* recordPtr = (*disk).writeRecord(record);
            tree->insert(record.fg3_pct_home, recordPtr);
            numOfRecords++; 
        }
        inputFile.close();

        return numOfRecords;
    }
}

#endif