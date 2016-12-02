#include "projection.h"

Projection::Projection (long double score, long double value, double salary, int production_rating, int matchup_rating, long double fppg, long double ownership, string opponent_team)
{
    this->score = score;
    this->value = value;
    this->salary = salary;
    this->production_rating = production_rating;
    this->matchup_rating = matchup_rating;
    this->fppg = fppg;
    this->ownership = ownership;
    this->opponent_team = opponent_team;
}

long double Projection::getScore ()
{
    return this->score;
}

double Projection::getSalary ()
{
    return this->salary;
}

long double Projection::getOwnerships ()
{
    return this->ownership;
}

Projection::~Projection()
{

}
