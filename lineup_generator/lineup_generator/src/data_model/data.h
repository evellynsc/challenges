#ifndef _DATA_H_
#define _DATA_H_

#include <string>
#include <map>
#include <vector>
#include <set>
#include "../data_model/player.h"
#include "../data_model/lineup_constraints.h"

using namespace std;

class Data {
    vector<Player> players_;
    LineupConstraints* lconstraints_;

    public:
        Data (vector<Player>, LineupConstraints*);
        // Projection* getProjection();
        vector<double> getAllScores ();
        set<string> getAllDistinctTeams ();
        vector<string> getAllTeams ();
        vector<double> getAllSalaries ();
        vector<double> getAllOwnerships ();
        map< string, vector<int> > getTeamPlayers ();
        map< string, vector<int> > getTeamDefenders ();
        map< string, vector<int> > getPositionPlayers ();
        int getNumPlayers();
        int getNumTeams();
        vector<Player> getPlayers();
        LineupConstraints* getConstraints();
};

#endif // _DATA_H_
