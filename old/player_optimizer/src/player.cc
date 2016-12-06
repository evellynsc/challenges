#include "player.h"

Player::Player (long int id, string first_name, string last_name, string position, string team, Projection* fanduel_projection)
{
    this->id = id;
    this->first_name = first_name;
    this->last_name = last_name;
    this->position = position;
    this->team = team;
    this->fanduel_projection = fanduel_projection;
}

long int Player::getId ()
{
    return this->id;
}

Player::~Player ()
{
    // delete fanduel_projection;
}

Projection* Player::getProjection ()
{
    return this->fanduel_projection;
}

string Player::getTeam()
{
    return this->team;
}

string Player::getPosition()
{
    return this->position;
}
