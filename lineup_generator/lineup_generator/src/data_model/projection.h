#ifndef _PROJECTION_H_
#define _PROJECTION_H_

#include <string>
using namespace std;

class Projection {

    double score;
    double value;
    double salary;
    int production_rating;
    int matchup_rating;
    double fppg;
    double ownership;
    string opponent_team;

    public:
        Projection (double, double, double, int, int, double, double, string);
        double getScore ();
        double getSalary ();
        double getOwnership ();
        ~Projection ();
};

#endif // _PROJECTION_H_
