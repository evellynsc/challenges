#include "lineup.h"

LineUp::LineUp (int num_players)
{
    this->num_players = num_players;
}

bool LineUp::setPlayers (vector<Player> players)
{
    if (players.size () != (unsigned) this->num_players)
    {
        return false;
    }
    this->players = players;
    return true;
}
