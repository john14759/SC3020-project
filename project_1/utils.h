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
    int dateStringToDaysSinceEpoch(std::string date) {
        istringstream dateStream(date);
        int day, month, year;
        char delimiter;
        dateStream >> day >> delimiter >> month >> delimiter >> year;
        date::year_month_day ymd{date::year(year), date::month(month), date::day(day)};
        return date::sys_days{ymd}.time_since_epoch().count();
    }

    int readFileIntoDisk(std::string fileName, Disk* disk) {
        ifstream inputFile;
        inputFile.open(fileName);
        if (inputFile.is_open()) {
            std::cout << "File is Open" << std::endl;
        } else {
            std::cout << "File failed to Open" << std::endl;
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
            if (PTS_home == "") {
                continue;
            }

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

            Record* recordPtr = (*disk).writeRecord(record);
            
            // Insert record to tree here!!!!
            // cout << record.game_date_est << endl;
            // cout << record.team_id_home << endl;
            // cout << int(record.pts_home) << endl;
            // cout << record.fg_pct_home << endl;
            // cout << record.ft_pct_home << endl;
            // cout << record.fg3_pct_home << endl;
            // cout << int(record.ast_home) << endl;
            // cout << int(record.reb_home) << endl;
            // cout << record.home_team_wins << endl;
            // cout << endl;
            numOfRecords++;
            // if (numOfRecords == 50)
            //     break;
        }
        inputFile.close();
        return numOfRecords;
    }
}

#endif