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
    inputFile.open(fileName); // Open the input file
    
    if (inputFile.is_open()) {
        // File is opened successfully
    } else {
        std::cout << "File failed to Open" << std::endl;
    }
    
    string line;
    int numOfRecords = 0;
    getline(inputFile, line); // Read and discard the first line
    
    // Read each line of the file
    while (getline(inputFile, line)) {
        istringstream iss(line); // Create a string stream to parse the line
        
        // Variables to store the values from each field in the line
        string GAME_DATE_EST, TEAM_ID_home, PTS_home, FG_PCT_home, FT_PCT_home;
        string FG3_PCT_home, AST_home, REB_home, HOME_TEAM_WINS;
        
        // Extract the values from each field in the line
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
        if (PTS_home == "") {
            continue; // Skip this record and move to the next line
        }
        
        // Create a new Record object with the extracted values
        Record record = {
            usint(dateStringToDaysSinceEpoch(GAME_DATE_EST)),
            stoul(TEAM_ID_home),
            static_cast<char>(stoi(PTS_home)),
            stof(FG_PCT_home),
            stof(FT_PCT_home),
            stof(FG3_PCT_home),
            static_cast<char>(stoi(AST_home)),
            static_cast<char>(stoi(REB_home)),
            HOME_TEAM_WINS == "1" ? true : false
        };
        
        // Write the record to the disk and get a pointer to it
        Record* recordPtr = (*disk).writeRecord(record);
        
        // Insert the record into the BPTree using the fg3_pct_home as the key
        tree->insert(record.fg3_pct_home, recordPtr);
        
        numOfRecords++; // Increment the number of records processed
        
        // Uncomment the following lines to print the record values
        /*
        cout << record.game_date_est << " ";
        cout << record.team_id_home << " ";
        cout << int(record.pts_home) << " ";
        cout << record.fg_pct_home << " ";
        cout << record.ft_pct_home << " ";
        cout << record.fg3_pct_home << " ";
        cout << int(record.ast_home) << " ";
        cout << int(record.reb_home) << " ";
        cout << record.home_team_wins << endl;
        cout << endl;
        */
        
        // Uncomment the following lines to limit the number of records processed
        /*
        if (numOfRecords == 26000)
            break;
        */
    }
    
    inputFile.close(); // Close the input file
    
    return numOfRecords; // Return the total number of records processed
}
}

#endif