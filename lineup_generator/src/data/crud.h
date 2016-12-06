#ifndef _CRUD_H_
#define _CRUD_H_

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <map>
#include <pqxx/pqxx>
#include "../data_model/projection.h"
#include "../data_model/player.h"
#include "../data_model/lineup_constraints.h"

using namespace std;
using namespace pqxx;

class CRUD {
    connection* connection_;

    public:
        CRUD (string, string, string, string, string);
        vector<Player> retrieveAllPlayers ();
        LineupConstraints retriveConstraint ();

        bool create (string);
        void closeConnection ();
        void saveLineup ();
        void saveAllLineups (vector< vector< pair<int, string> > >);

};

#endif // _CRUD_H_
