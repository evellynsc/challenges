#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <string>
#include "projection.h"

using namespace std;

class Player {

    int id;
    string first_name;
    string last_name;
    string position;
    string team;
    Projection* fanduel_projection;

    public:
        Player (int, string, string, string, string, Projection*);
        Projection* getProjection();
        string getTeam();
        string getPosition();
        int getId();
        ~Player ();
};

#endif // _PLAYER_H_
