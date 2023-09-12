#ifndef STRUCTS_H
#define STRUCTS_H

typedef unsigned int uint;
typedef unsigned short int usint;

struct Record {
    uint game_date_est;
    uint team_id_home;
    usint pts_home;
    float fg_pct_home;
    float ft_pct_home;
    float ft3_pct_home;
    usint ast_home;
    usint reb_home;
    bool home_team_wins;
};

#endif 