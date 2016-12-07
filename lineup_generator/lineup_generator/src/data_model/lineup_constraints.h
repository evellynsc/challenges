#ifndef _LINEUPCONSTRAINTS_H_
#define _LINEUPCONSTRAINTS_H_

struct LineupConstraints {
    double min_salary_sum;
    double max_salary_sum;
    double ownership_sum;
    int min_distinct_teams;
    int max_players_from_same_team;
    int num_players;
    map<string, int> position_players;
};

#endif
