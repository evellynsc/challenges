#include "base/commandlineflags.h"
#include "data_model/player.h"
#include "math_model/model.h"
#include "data/crud.h"

#include <iostream>
#include <fstream>
#include <vector>


DEFINE_int32(num_lineups, 1, "Number of lineups to generate.");
DEFINE_string(dbname, "", "Database name.");
DEFINE_string(user, "", "User name to access database.");
DEFINE_string(password, "", "Password to access database.");
DEFINE_string(host, "127.0.0.1", "Host address to access database.");
DEFINE_string(port, "5432", "Port number to access database.");

using namespace std;

/* Test if a given lineup is valid */
bool validate_lineup (vector<Player> players, LineupConstraints* constraints)
{
    int num_players = constraints->num_players;
    if (players.size() != num_players)
    {
        cout << "numero de jogadores" << endl;
        return false;
    }

    double ownership_sum = 0.0;
    double salary_sum = 0.0;
    int players_from_same_team = 0.0;
    map<string, int> team_players = {};
    map<string, int> position_players = {};
    string team_defender;

    for (int i = 0; i < num_players; ++i)
    {
        salary_sum += players[i].getProjection()->getSalary();
        ownership_sum += players[i].getProjection()->getOwnership()*100;
        string team = players[i].getTeam();
        string position = players[i].getPosition();

        if (position.compare("D") == 0)
        {
            team_defender = team;
        }

        if (position_players.find(position) != position_players.end())
        {
            position_players[position] = 1;
        } else
        {
            position_players[position] += 1;
            if (position_players[position] > constraints->position_players[position])
            {
                cout << position << " has " << position_players[position] << endl;
                return false;
            }
        }

        if (team_players.find(team) != team_players.end())
        {
            team_players[team] = 1;
        } else
        {
            team_players[team] += 1;
            if (team_players[team] > constraints->max_players_from_same_team)
            {
                cout << "players from same team " << team_players[team] << endl;
                return false;
            }
        }
    }
    if (team_players[team_defender] != 1)
    {
        cout << "defender" << endl;
        return false;
    }

    cout << "salary " << salary_sum << endl;
    cout << "ownership " << ownership_sum << endl;
    cout << "times diferentes " <<team_players.size() << endl;
    if (salary_sum >= constraints->min_salary_sum &&
        salary_sum <= constraints->max_salary_sum &&
        ownership_sum >= constraints->ownership_sum &&
        team_players.size() >= constraints->min_distinct_teams)
    {
        return true;
    }
    cout << "outra coisa!" << endl;
    return false;
}

int main (int argc, char** argv) {
    gflags::ParseCommandLineFlags (&argc, &argv, true);

    if (FLAGS_dbname.compare("") == 0 || FLAGS_user.compare("") == 0 || FLAGS_password.compare("") == 0) {
        cout << "Missing arguments." << endl;
        cout << "Usage: ./player_optimizer [-num_lineups n] -dbname <db_name> -user <db_user> -password <db_password> [-host db_address] [-port db_port]" << endl;
        exit(1);
    }

    vector< vector< pair <int, string> > > lineups;
    vector<Player> players;
    LineupConstraints lconstraints;
    CRUD* database;

    /* Open database connection */
    database = new CRUD (FLAGS_dbname, FLAGS_user, FLAGS_password, FLAGS_host, FLAGS_port);

    /* Retrieve data from database */
    players = database->retrieveAllPlayers ();
    lconstraints = database->retriveConstraint ();

    /* Create model */
    Data* player_data = new Data (players, &lconstraints);
    Model* lineup_model = new Model(player_data, MAXIMIZE, "lineup_model");
    lineup_model->createModel();

    /* Optimize model to get the best solution */
    lineup_model->optimize();

    /* Get best solution */
    lineups.push_back (lineup_model->getSolution());

    /* Reoptimize model to get other solutions */
    for (int i = 0; i < FLAGS_num_lineups - 1; ++i)
    {
        lineup_model->reoptimize();
        lineups.push_back (lineup_model->getSolution());
    }

    /* Print time to find each solution */
    // lineup_model->print_times();
    database->saveAllLineups(lineups);

    /* Free memory */
    // TODO: comment if taking too long
    players.clear();
    delete lineup_model;

    /* Close the database connection, not necessary anymore */
    database->closeConnection ();

    return 0;
}
