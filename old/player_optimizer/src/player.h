#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <string>
#include "projection.h"

using namespace std;

class Player {

    long int id;
    string first_name;
    string last_name;
    string position;
    string team;
    Projection *fanduel_projection;

    public:
        Player (long int, string, string, string, string, Projection*);
        Projection* getProjection();
        string getTeam();
        string getPosition();
        long int getId();
        ~Player ();
};

#endif // _PLAYER_H_
