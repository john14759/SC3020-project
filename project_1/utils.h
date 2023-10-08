#ifndef UTILS_H
#define UTILS_H

#include "lib/date.h"
#include "storage/disk.h"
#include "tree/b_plus_tree.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

namespace utils {
    // Define a function to convert a date string to the number of days since epoch
    int dateStringToDaysSinceEpoch(std::string date) {
        // Create an input string stream from the date string
        istringstream dateStream(date);
        // Declare variables to store the day, month, and year
        int day, month, year;
        // Declare a variable to store the delimiter
        char delimiter;
         // Extract the day, month, and year from the date string using the delimiter
        dateStream >> day >> delimiter >> month >> delimiter >> year;
        // Create a date object from the day, month, and year
        date::year_month_day ymd{date::year(year), date::month(month), date::day(day)};
        // Convert the date to the number of days since epoch and return the result
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
            string GAME_DATE_EST, TEAM_ID_home, PTS_home, FG_PCT_home, FT_PCT_home, FG3_PCT_home, AST_home, REB_home, HOME_TEAM_WINS;
            getline(iss, GAME_DATE_EST, '\t');
            getline(iss, TEAM_ID_home, '\t');
            getline(iss, PTS_home, '\t');
            getline(iss, FG_PCT_home, '\t');
            getline(iss, FT_PCT_home, '\t');
            getline(iss, FG3_PCT_home, '\t');
            getline(iss, AST_home, '\t');
            getline(iss, REB_home, '\t');
            getline(iss, HOME_TEAM_WINS, '\t');

            // Check if the record is relevant (PTS_home field should not be empty)
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
        inputFile.close(); // Close the input file
        return numOfRecords; // Return the total number of records processed
    }
}

#endif