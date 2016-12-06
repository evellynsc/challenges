#include "projection.h"

Projection::Projection (double score, double value, double salary, int production_rating, int matchup_rating, double fppg, double ownership, string opponent_team)
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

double Projection::getScore ()
{
    return this->score;
}

double Projection::getSalary ()
{
    return this->salary;
}

double Projection::getOwnership ()
{
    return this->ownership;
}

Projection::~Projection()
{

}
