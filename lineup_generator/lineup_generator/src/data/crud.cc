#include "crud.h"

CRUD::CRUD (string dbname, string user, string password, string hostaddr, string port)
{
    try
    {
        this->connection_ = new connection("dbname=" + dbname + " user=" + user + " password=" + password + " hostaddr=" + hostaddr + " port=" + port);

        if (!this->connection_->is_open())
        {
            cout << "Can't open database" << endl;
            exit(1);
        }

        cout << "Opened database successfully: " << this->connection_->dbname() << endl;
    } catch (const std::exception &e)
    {
        cerr << e.what() << std::endl;
        exit(1);
    }
}

LineupConstraints CRUD::retriveConstraint ()
{
    LineupConstraints lconstraints = {};
    try{
        /* Create a non-transactional object. */
        nontransaction N (*this->connection_);
        /* Execute SQL query */
        result R_constraint (N.exec ("SELECT * FROM constraints;"));


        /* List down all the records */
        lconstraints.min_salary_sum = R_constraint[0][1].as<double>();
        lconstraints.max_salary_sum = R_constraint[0][2].as<double>();
        lconstraints.ownership_sum = R_constraint[0][3].as<double>();
        lconstraints.min_distinct_teams = R_constraint[0][4].as<int>();
        lconstraints.max_players_from_same_team = R_constraint[0][5].as<int>();
        lconstraints.num_players = R_constraint[0][6].as<int>();

        map<string, int> position_players;
        position_players["QB"] = R_constraint[0][7].as<int>();
        position_players["D"] = R_constraint[0][12].as<int>();
        position_players["TE"] = R_constraint[0][8].as<int>();
        position_players["K"] = R_constraint[0][11].as<int>();
        position_players["RB"] = R_constraint[0][9].as<int>();
        position_players["WR"] = R_constraint[0][10].as<int>();

        lconstraints.position_players = position_players;

        return lconstraints;

    }catch (const std::exception &e){
        cerr << e.what() << std::endl;
        return lconstraints;
    }
}


vector<Player> CRUD::retrieveAllPlayers ()
{
    vector<Player> players;
    try{
        /* Create a non-transactional object. */
        nontransaction N (*this->connection_);
        /* Execute SQL query */
        result R_players (N.exec ("SELECT * FROM players ORDER BY fanduel_id;"));

        result R_projections (N.exec ("SELECT * FROM fanduel_projections ORDER BY id;"));

        /* List down all the records */
        if (R_projections.size () == R_players.size ())
        {
            for (result::const_iterator c1 = R_players.begin(), c2 = R_projections.begin(); c1 != R_projections.end(), c2 != R_projections.end(); ++c1, ++c2)
            {
                Projection* projection = new Projection(c2[1].as<double>(), c2[2].as<double>(), c2[3].as<double>(), c2[4].as<int>(), c2[5].as<int>(), c2[6].as<double>(), c2[7].as<double>(), c2[8].as<string>());

                Player player(c1[0].as<int>(), c1[1].as<string>(), c1[2].as<string>(), c1[3].as<string>(), c1[4].as<string>(), projection);

                players.push_back (player);
            }
        }

        return players;
    }catch (const std::exception &e){
        cerr << e.what() << std::endl;
        return players;
    }
}

void CRUD::closeConnection (){
    this->connection_->disconnect();
}


void CRUD::saveAllLineups (vector< vector< pair<int, string> > > lineups)
{
    try{
        stringstream insert_sql_command;
        string insert_sql_values_command = "";

        /* iterate for each lineup */
        for (vector< vector< pair<int, string> > >::iterator itl = lineups.begin(); itl != lineups.end(); ++itl)
        {
            int num_rb = 1;
            int num_wr = 1;
            int num_players = 1;

            insert_sql_command << "INSERT INTO lineups (";
            /* iterate for each player from the lineup */
            for (vector< pair<int, string> >::iterator itp = itl->begin(); itp != itl->end(); ++itp)
            {
                int player_id = itp->first;
                string player_position = itp->second;

                if (player_position.compare("QB") == 0)
                {
                    insert_sql_command << "qb_id";
                    insert_sql_values_command += to_string(player_id);
                }
                else if (player_position.compare("RB") == 0)
                {
                    insert_sql_command << "rb" << num_rb << "_id";
                    insert_sql_values_command += to_string(player_id);
                    num_rb++;
                }
                else if (player_position.compare("WR") == 0)
                {
                    insert_sql_command << "wr" << num_wr << "_id";
                    insert_sql_values_command += to_string(player_id);
                    num_wr++;
                }
                else if (player_position.compare("TE") == 0)
                {
                    insert_sql_command << "te_id";
                    insert_sql_values_command += to_string(player_id);
                }
                else if (player_position.compare("K") == 0)
                {
                    insert_sql_command << "k_id";
                    insert_sql_values_command += to_string(player_id);
                }
                else if (player_position.compare("D") == 0)
                {
                    insert_sql_command << "d_id";
                    insert_sql_values_command += to_string(player_id);
                }

                if (num_players < itl->size())
                {
                    insert_sql_command << ",";
                    insert_sql_values_command += ",";
                }
                num_players++;

            }/* iterate for each player from the lineup */

            insert_sql_command << ") VALUES (" << insert_sql_values_command << ");" << endl;
            insert_sql_values_command = "";

        }/* iterate for each lineup */

        /* Create a transactional object. */
        work W(*this->connection_);
        /* Execute SQL query */
        W.exec(insert_sql_command.str());
        W.commit();

    }catch (const std::exception &e){
        cerr << e.what() << std::endl;
    }
}
