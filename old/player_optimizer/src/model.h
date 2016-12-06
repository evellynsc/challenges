#ifndef _MODEL_H_
#define _MODEL_H_

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <fstream>

#include "data.h"

#define MINIMIZE false
#define MAXIMIZE true
#define TOLERANCE 0.00001

using namespace std;

class Model {

    long double current_objective_value_;

    string model_name_;
    string objective_;
    string variables_;
    string constraints_;

    map< string, vector<string> > variable_map_;
    map< long int, string > player_id_var_map_;
    map< string, string > team_var_map_;

    bool sense_;
    Data* data_;

    // returns the number of players in the lineup for a given position
    int getPositionLimit(string);

    void createObjective ();
    void createVariables ();
    void createConstraints ();
    map< string, vector<string> > getTeamPlayerVars ();
    map< string, vector<string> > getPositionPlayerVars ();
    map< string, vector<string> > getTeamDefenderVars ();



    public:
        Model (Data*, bool, string);
        void createModel ();
        void saveModel ();
        void optimize ();
        void optimize (int);
        vector<Player> getSolution ();
        ~Model ();
};

#endif // _MODEL_H_
