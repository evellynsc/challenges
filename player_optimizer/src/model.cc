#include "model.h"

Model::Model (Data* data, bool sense, string model_name)
{
    this->data_ = data;
    this->sense_ = sense;
    this->model_name_ = model_name;
}

map< string, vector<string> > Model::getTeamPlayerVars ()
{
    // maps each team to a vector of its player ids
    map< string, vector<long int> > team_player_ids = this->data_->getTeamPlayers();
    map< string, vector<string> > team_player_vars;

    for (map< string, vector<long int> >::iterator it = team_player_ids.begin(); it != team_player_ids.end(); ++it)
    {
        vector<long int> player_ids = it->second;
        vector<string> player_vars;
        int num_players = player_ids.size ();

        for (int j = 0; j < num_players; ++j)
        {
            player_vars.push_back (this->player_id_var_map_[player_ids[j]]);
        }

        team_player_vars[it->first] = player_vars;
    }
    return team_player_vars;
}

map< string, vector<string> > Model::getPositionPlayerVars ()
{
    map< string, vector<long int> > position_player_ids = this->data_->getPositionPlayers();
    map< string, vector<string> > position_player_vars;

    for (map< string, vector<long int> >::iterator it = position_player_ids.begin(); it != position_player_ids.end(); ++it)
    {
        vector<long int> player_ids = it->second;
        vector<string> player_vars;
        int num_players = player_ids.size ();

        for (int j = 0; j < num_players; ++j)
        {
            player_vars.push_back (this->player_id_var_map_[player_ids[j]]);
        }

        position_player_vars[it->first] = player_vars;
    }

    return position_player_vars;
}

map< string, vector<string> > Model::getTeamDefenderVars ()
{
    map< string, vector<long int> > team_defender_ids = this->data_->getTeamDefenders();
    map< string, vector<string> > team_defender_vars;

    for (map< string, vector<long int> >::iterator it = team_defender_ids.begin(); it != team_defender_ids.end(); ++it)
    {
        vector<long int> defender_ids = it->second;
        vector<string> defender_vars;
        int num_defenders = defender_ids.size ();

        for (int j = 0; j < num_defenders; ++j)
        {
            defender_vars.push_back (this->player_id_var_map_[defender_ids[j]]);
        }

        team_defender_vars[it->first] = defender_vars;
    }

    return team_defender_vars;
}

// Model::Model (vector<Player> players, LineupConstraints* lineup_constraints, bool sense, string model_name)
// {
//     this->players_ = players;
//     lconstraints = lineup_constraints;
//     this->sense_ = sense;
//     this->model_name_ = model_name;
//     this->current_objective_value_ = -1;
// }


void Model::createObjective()
{
    string aux;
    vector<long double> scores = this->data_->getAllScores ();
    int num_terms = scores.size ();
    this->objective_ = "" + to_string (scores[0]) + " " + this->variable_map_["player"][0];
    for (int i = 1; i < num_terms; ++i)
    {
        this->objective_ += " + " + to_string (scores[i]) + " " + this->variable_map_["player"][i];
    }
}

void Model::createVariables()
{
    this->variables_ = "";
    vector<string> player_vars;
    vector<string> team_vars;

    vector<Player> players = this->data_->getPlayers();
    int num_players = players.size ();
    set<string> teams = this->data_->getAllDistinctTeams ();

    for (int i = 0; i < num_players; ++i)
    {
        player_vars.push_back("player_" + to_string (i));
        this->player_id_var_map_[players[i].getId()] = player_vars[i];
        this->variables_ += "player_" + to_string (i) + "\n";
    }
    
    for (set<string>::iterator it = teams.begin(); it != teams.end(); ++it)
    {
        team_vars.push_back("team_" + *it);
        this->team_var_map_[*it] = "team_" + *it;
        this->variables_ += "team_" + *it + "\n";
    }
    this->variable_map_["player"] = player_vars;
    this->variable_map_["team"] = team_vars;
}

void Model::createConstraints()
{
    this->constraints_ = "";
    // ==================== salary and ownership constraints =============
    vector<double> salaries = this->data_->getAllSalaries ();
    vector<long double> ownerships = this->data_->getAllOwnerships ();
    int num_players = this->data_->getNumPlayers ();
    vector<string> player_vars = this->variable_map_["player"];
    LineupConstraints lconstraints = this->data_->getConstraints ();

    string salary_constraint = "" + to_string (salaries[0]) + " " + player_vars[0];
    string ownership_constraint = "" + to_string (ownerships[0]*100) + " " + player_vars[0];


    for (int i = 1; i < num_players; ++i)
    {
        salary_constraint += " + " + to_string (salaries[i]) + " " + player_vars[i];
        ownership_constraint += " + " + to_string (ownerships[i]*100) + " " + player_vars[i];
    }

    this->constraints_ += salary_constraint + " >= " + to_string (lconstraints.min_salary_sum) + "\n";
    this->constraints_ += salary_constraint + " <= " + to_string (lconstraints.max_salary_sum) + "\n";
    this->constraints_ += ownership_constraint + " >= " + to_string (lconstraints.ownership_sum) + "\n";

    // ============================ team constraints =====================
    vector<string> team_vars = this->variable_map_["team"];
    set<string> teams = this->data_->getAllDistinctTeams ();
    map< string, vector<string> > team_players = this->getTeamPlayerVars ();
    map< string, vector<string> > team_defenders = getTeamDefenderVars ();
    string sum_team_vars = team_vars[0];
    int i = 0;

    for (set<string>::iterator it = teams.begin(); it != teams.end(); ++it)
    {
        vector<string> player_vars = team_players[*it];
        vector<string> defender_vars = team_defenders[*it];

        string sum_player_vars = player_vars[0];
        string sum_defender_vars = "";

        if (defender_vars.size () > 0)
        {
            sum_defender_vars = defender_vars[0];
        }

        int num_players = player_vars.size ();
        int num_defenders = defender_vars.size ();

        for (int j = 1; j < num_players; ++j)
        {
            sum_player_vars += " + " + player_vars[j];
            this->constraints_ += team_vars[i] + " >= " + player_vars[j] + "\n";

            if (j < num_defenders)
            {
                sum_defender_vars += " + " + defender_vars[j];
            }
        }

        this->constraints_ += sum_player_vars + " <= " + to_string(lconstraints.max_players_from_same_team) + "\n";

        if (defender_vars.size () > 0)
        {
            this->constraints_ += sum_defender_vars + " + " + team_vars[i] + " <= 1" + "\n";
        }
        this->constraints_ += team_vars[i] + " <= " + sum_player_vars + "\n";

        if (i != 0)
        {
            sum_team_vars += " + " + team_vars[i];
        }
        ++i;
    }
    this->constraints_ += sum_team_vars + " >= " + to_string(lconstraints.min_distinct_teams) + "\n";

    // =========== number of player in each position constraints ========
    map< string, vector<string> > position_players = getPositionPlayerVars ();
    for (map< string, vector<string> >::iterator it = position_players.begin(); it != position_players.end(); ++it)
    {
        vector<string> player_vars = it->second;
        string sum_player_vars = player_vars[0];
        int num_players = player_vars.size ();

        for (int i = 0; i < num_players; ++i)
        {
            sum_player_vars += " + " + player_vars[i];
        }

        this->constraints_ += sum_player_vars + " = " + to_string(this->getPositionLimit (it->first)) + "\n";
    }
}

int Model::getPositionLimit (string position)
{
    if (position.compare("QB") == 0 || position.compare("K") == 0 || position.compare("TE") == 0 || position.compare("D") == 0)
        return 1;
    if (position.compare("RB") == 0)
        return 2;
    return 3;
}

void Model::createModel ()
{
    cout << "creating variables..." << endl;
    this->createVariables();

    cout << "creating objective..." << endl;
    this->createObjective();

    cout << "creating constraints..." << endl;
    this->createConstraints();
}

void Model::saveModel ()
{
    ofstream model_file;
    model_file.open ((this->model_name_ + ".lp").c_str());

    if (model_file.is_open())
    {
        if (this->sense_ == true)
        {
            model_file << "Maximize" << endl;
        } else {
            model_file << "Minimize" << endl;
        }

        model_file << "\t" << this->objective_ << endl;

        model_file << "Subject To" << endl;
        if (this->current_objective_value_ != -1)
        {
            this->constraints_ += objective_ + " <= " + to_string (this->current_objective_value_ - TOLERANCE) + "\n";
        }
        model_file << "\t" << this->constraints_ << endl;

        model_file << "Binary" << endl;
        model_file << this->variables_ << endl;

        model_file << "End";

        model_file.close();
    } else
    {
        cerr << "ERROR: Unable to open file to write model." << endl;
        exit(1);
    }

}

void Model::optimize(int num_solutions)
{
    if (system("cbc -help") != 0)
    {
        cerr << "Solver cbc is not installed." << endl;
        exit(1);
    }
    string solutions = "";
    for (int i = 0; i < num_solutions; ++i)
    {
        solutions += "-nextbest sol-" + to_string (i) + ".txt ";
    }

    string command = "cbc -import " + this->model_name_ + ".lp" + " -solv -solu " + this->model_name_ + ".txt " + solutions;
    system(command.c_str());
}

void Model::optimize()
{
    if (system("cbc -help") != 0)
    {
        cerr << "Solver cbc is not installed." << endl;
        exit(1);
    }
    string command = "cbc -import " + this->model_name_ + ".lp" + " -solv -solu " + this->model_name_ + ".txt";
    system(command.c_str());
}

vector<Player> Model::getSolution ()
{
    ifstream model_file;
    string line;
    vector<Player> selected_players;
    int idx_player;
    model_file.open ((this->model_name_ + ".txt").c_str());
    if (model_file.is_open())
    {
        model_file >> line;
        // if model is not infeasible then read solution
        if (line.find ("Infeasible") == string::npos)
        {
            // Discarding all unnecessary information
            for (int i = 0; i < 3; i++)
            {
                model_file >> line;
                cout << line << endl;
            }
            cout << "estou aqui" << endl;
            model_file >> this->current_objective_value_;
            this->current_objective_value_ *= -1.0;
            // cout << line << this->current_objective_value_;
            // this->current_objective_value_ = stold (line);
            cout << "objective value: " << this->current_objective_value_ << endl;

            int count_players = 0;
            while (count_players < 9)
            {
                ++count_players;
                model_file >> idx_player;
                // selected_players.push_back (this->players_[idx_player]);
                cout << "player: " << idx_player << endl;
                // Discarding all unnecessary information
                for (int i = 0; i < 3; i++)
                {
                    model_file >> line;
                }
            }
            return selected_players;
        } else {
            cerr << "ERROR: Model is infeasible." << endl;
            exit(1);
        }

    } else
    {
        cerr << "ERROR: Unable to open file to read solution." << endl;
        exit(1);
    }
}
