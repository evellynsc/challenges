#ifndef _PROJECTION_H_
#define _PROJECTION_H_

#include <string>
using namespace std;

class Projection {

    long double score;
    long double value;
    double salary;
    int production_rating;
    int matchup_rating;
    long double fppg;
    long double ownership;
    string opponent_team;

    public:
        Projection (long double, long double, double, int, int, long double, long double, string);
        long double getScore ();
        double getSalary ();
        long double getOwnerships ();
        ~Projection ();
};

#endif // _PROJECTION_H_
