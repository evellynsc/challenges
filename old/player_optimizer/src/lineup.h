#ifndef _LINEUP_H_
#define _LINEUP_H_

#include <string>
#include <map>
#include <vector>

#include "player.h"

using namespace std;

struct Attributes {
    double salary_sum;
    double score_sum;
    double ownership_sum;
};

class LineUp {

    int num_players;
    vector<Player> players;
    Attributes attributes;

    public:
        LineUp (int num_players);
        bool setPlayers (vector<Player>);
        void calculateAttributes ();
};

#endif // _LINEUP_H_
