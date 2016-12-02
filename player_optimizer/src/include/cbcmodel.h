#ifndef _CBCMODEL_H_
#define _CBCMODEL_H_

#include <string>
#include <map>
#include <vector>

using namespace std;

class CBCModel {

    bool maximize;
    map< string, vector<string> > variables;
    string objective;
    vector<string> constraints;

    void create_objective (vector<double>, string);
    void create_variables (string, int);
    void create_salary_and_ownership_constraints (vector<double>, vector<long double>, string);
    void create_team_constraints (vector<string>, map< string, vector<string> >, string);

    public:
        Model ();
        void create_model (vector<Player>);
        void save (string);
        ~Model ();
};

#endif // _CBCMODEL_H_
