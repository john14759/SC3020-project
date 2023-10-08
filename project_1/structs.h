#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>

typedef unsigned short int usint;
typedef unsigned int uint;
typedef unsigned char uchar;

struct Record {
    float fg_pct_home;
    float ft_pct_home;
    float fg3_pct_home;
    uint team_id_home;
    usint game_date_est; // days since epoch 
    char pts_home;
    char ast_home;
    char reb_home;
    bool home_team_wins;
};

struct Node {
    bool isLeaf;
    std::vector<float> keys;
    std::vector<Node*> ptrs;
    std::vector<std::vector<Record*>> records;
    Node* nextNodePtr;

    Node(bool isLeaf) {
        this->isLeaf = isLeaf;
    }
};

#endif 