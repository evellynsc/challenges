#include "data.h"

Data::Data(vector<Player> players, LineupConstraints* lconstraints)
{
    this->players_ = players;
    this->lconstraints_ = lconstraints;
}

vector<double> Data::getAllScores ()
{
    int num_players = this->players_.size ();
    vector<double> scores;
    for (int i = 0; i < num_players; ++i)
    {
        scores.push_back (this->players_[i].getProjection()->getScore());
    }
    return scores;
}

set<string> Data::getAllDistinctTeams ()
{
    int num_players = this->players_.size ();
    set<string> teams;
    for (int i = 0; i < num_players; ++i)
    {
        teams.insert (this->players_[i].getTeam());
    }
    return teams;
}

int Data::getNumPlayers ()
{
    return this->players_.size ();
}

int Data::getNumTeams ()
{
    return this->getAllDistinctTeams().size ();
}

vector<Player> Data::getPlayers ()
{
    return this->players_;
}

LineupConstraints* Data::getConstraints ()
{
    return this->lconstraints_;
}

vector<double> Data::getAllSalaries ()
{
    int num_players = this->players_.size ();
    vector<double> salaries;
    for (int i = 0; i < num_players; ++i)
    {
        salaries.push_back (this->players_[i].getProjection()->getSalary());
    }
    return salaries;
}

vector<double> Data::getAllOwnerships ()
{
    int num_players = this->players_.size ();
    vector<double> ownerships;
    for (int i = 0; i < num_players; ++i)
    {
        ownerships.push_back (this->players_[i].getProjection()->getOwnership());
    }
    return ownerships;
}

map< string, vector<int> > Data::getTeamPlayers ()
{
    int num_players = this->players_.size ();
    map< string, vector<int> > team_players;
    for (int i = 0; i < num_players; ++i)
    {
        string team = this->players_[i].getTeam();

        // if the team was already inserted into the map then add the
        // player into the corresponding team vector
        // otherwise insert the team into the map and initialize its vector
        // with the player
        if (team_players.find(team) != team_players.end())
        {
            team_players[team].push_back (this->players_[i].getId());
        }
        else
        {
            team_players[team] = vector<int> {this->players_[i].getId()};
        }

    }
    return team_players;
}

map< string, vector<int> > Data::getPositionPlayers ()
{
    int num_players = this->players_.size ();
    map< string, vector<int> > position_players;
    for (int i = 0; i < num_players; ++i)
    {
        string position = this->players_[i].getPosition();
        // if the team was already inserted into the map then add the
        // player variable into the corresponding team vector
        // otherwise insert the team into the map and initialize its vector
        // with the player variable
        if (position_players.find(position) != position_players.end())
        {
            position_players[position].push_back (this->players_[i].getId());
        }
        else
        {
            position_players[position] = vector<int> {this->players_[i].getId()};
        }

    }
    return position_players;
}

map< string, vector<int> > Data::getTeamDefenders ()
{
    int num_players = this->players_.size ();
    map< string, vector<int> > team_players;
    for (int i = 0; i < num_players; ++i)
    {
        // if current player is a defender add her/his variable into the map
        if (this->players_[i].getPosition().compare("D") == 0)
        {
            string team = this->players_[i].getTeam();
            // if the team was already inserted into the map then add the
            // player variable into the corresponding team vector
            // otherwise insert the team into the map and initialize its vector
            // with the player variable
            if (team_players.find(team) != team_players.end())
            {
                team_players[team].push_back (this->players_[i].getId());
            }
            else
            {
                team_players[team] = vector<int> {this->players_[i].getId()};
            }
        }
    }
    return team_players;
}
