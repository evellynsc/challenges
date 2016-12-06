#ifndef _DATA_H_
#define _DATA_H_

#include <string>
#include <map>
#include <vector>
#include <set>
#include "player.h"
#include "lineup_constraints.h"

using namespace std;

class Data {
    vector<Player> players_;
    LineupConstraints lconstraints_;

    public:
        Data (vector<Player>, LineupConstraints);
        // Projection* getProjection();
        vector<long double> getAllScores ();
        set<string> getAllDistinctTeams ();
        vector<string> getAllTeams ();
        vector<double> getAllSalaries ();
        vector<long double> getAllOwnerships ();
        map< string, vector<long int> > getTeamPlayers ();
        map< string, vector<long int> > getTeamDefenders ();
        map< string, vector<long int> > getPositionPlayers ();
        int getNumPlayers();
        int getNumTeams();
        vector<Player> getPlayers();
        LineupConstraints getConstraints();
};

#endif // _DATA_H_
