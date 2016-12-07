#ifndef _MODEL_H_
#define _MODEL_H_

#include "base/logging.h"
#include "linear_solver/linear_solver.h"

#include "../data/data.h"

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>

#define MINIMIZE false
#define MAXIMIZE true
#define TOLERANCE 0.00001

using namespace std;
using namespace operations_research;

class Model {

    bool sense_;
    MPSolver* solver_;
    Data* data_;
    int num_players_;
    int num_teams_;

    vector<MPVariable*> player_vars_;
    vector<MPVariable*> team_vars_;
    vector<MPConstraint*> projections_constraints_;
    vector<MPConstraint*> team_constraints_;
    vector<MPConstraint*> position_players_constraints_;
    vector<MPConstraint*> lineup_constraints_;


    map<int, int> player_to_idx_;
    vector<string> idx_to_team_;
    vector<int64> solve_time_;
    vector< pair<int, string> > selected_players_;


    public:
        Model (Data*, bool, string);
        ~Model ();
        void createVariables ();
        void createObjective ();
        void createConstraints ();
        void createModel ();
        void optimize();
        void reoptimize();
        void print_times();
        vector< pair<int, string> > getSolution();
};

#endif // _MODEL_H_
