#include "model.h"
#include "linear_solver/linear_solver.h"

#define EPSILON 0.00001


Model::~Model ()
{
    delete this->solver_;
    delete this->data_;
}

Model::Model (Data* data, bool sense, string model_name)
{
    this->data_ = data;
    this->num_players_ = this->data_->getNumPlayers ();
    this->num_teams_ = this->data_->getNumTeams ();
    this->sense_ = sense;
    // cbc solver initialization via google or-tools
    this->solver_ = new MPSolver(model_name, MPSolver::CBC_MIXED_INTEGER_PROGRAMMING);

    // this->solver_ = new MPSolver(model_name, MPSolver::SCIP_MIXED_INTEGER_PROGRAMMING);

    this->solve_time_.push_back (0);
}

void Model::createVariables ()
{
    this->player_to_idx_ = map<int, int>();

    vector<Player> players = this->data_->getPlayers();
    set<string> teams = this->data_->getAllDistinctTeams ();

    for (int i = 0; i < this->num_players_; ++i)
    {
        this->player_vars_.push_back (this->solver_->MakeIntVar(0.0, 1.0, "player_" + to_string(players[i].getId())));
        this->player_to_idx_[players[i].getId()] = i;
    }

    for (set<string>::iterator it = teams.begin(); it != teams.end(); ++it)
    {
        this->team_vars_.push_back (this->solver_->MakeIntVar(0.0, 1.0, "team_" + *it));
        this->idx_to_team_.push_back(*it);
    }
}

void Model::createConstraints ()
{
    // =========== salary and ownership constraints =====================
    vector<double> salaries = this->data_->getAllSalaries ();
    vector<double> ownerships = this->data_->getAllOwnerships ();
    LineupConstraints* lconstraints = this->data_->getConstraints ();
    const double infinity = this->solver_->infinity();

    MPConstraint* salary_sum_constraint = this->solver_->MakeRowConstraint(lconstraints->min_salary_sum, lconstraints->max_salary_sum);
    MPConstraint* ownership_sum_constraint = this->solver_->MakeRowConstraint(lconstraints->ownership_sum, infinity);

    this->projections_constraints_.push_back(salary_sum_constraint);
    this->projections_constraints_.push_back(ownership_sum_constraint);

    for (int i = 0; i < this->num_players_; ++i)
    {
        salary_sum_constraint->SetCoefficient(this->player_vars_[i], salaries[i]);
        ownership_sum_constraint->SetCoefficient(this->player_vars_[i], ownerships[i]*100);
    } // =========== salary and ownership constraints ==================

    //====================== team constraints =========================
    map< string, vector<int> > team_players = this->data_->getTeamPlayers();
    map< string, vector<int> > team_defenders = this->data_->getTeamDefenders();

    MPConstraint* const distinct_teams_constraint = this->solver_->MakeRowConstraint(lconstraints->min_distinct_teams, infinity);
    team_constraints_.push_back (distinct_teams_constraint);
    for (int i = 0; i < this->num_teams_; ++i)
    {
        distinct_teams_constraint->SetCoefficient(this->team_vars_[i], 1);

        // get players for corresponding team i
        vector<int> player_ids = team_players[this->idx_to_team_[i]];
        int num_player_ids = player_ids.size ();

        // get defender for corresponding team i
        vector<int> defender_ids = team_defenders[this->idx_to_team_[i]];
        int num_defender_ids = defender_ids.size ();

        MPConstraint* players_from_same_team_constraint = NULL;
        if (num_player_ids > 4)
        {
            players_from_same_team_constraint = this->solver_->MakeRowConstraint(-infinity, lconstraints->max_players_from_same_team);
            this->team_constraints_.push_back (players_from_same_team_constraint);
        }


        // the constraint is initialize only if there is at least one defender for team i
        MPConstraint* defender_constraint = NULL;
        if (num_defender_ids > 0)
        {
            defender_constraint = this->solver_->MakeRowConstraint(-infinity, 1);
            defender_constraint->SetCoefficient(this->team_vars_[i], 1);
            this->team_constraints_.push_back (defender_constraint);
        }

        // TODO: change variable name to reflect right meaning if necessary
        MPConstraint* const team_upper_bound = this->solver_->MakeRowConstraint(0.0, infinity);
        team_upper_bound->SetCoefficient(this->team_vars_[i], 1);
        this->team_constraints_.push_back (team_upper_bound);

        // TODO: change variable name to reflect right meaning if necessary
        MPConstraint* const team_lower_bound = this->solver_->MakeRowConstraint(-infinity, 0.0);
        team_lower_bound->SetCoefficient(this->team_vars_[i], 1);
        this->team_constraints_.push_back (team_lower_bound);

        for (int j = 0; j < num_player_ids; ++j)
        {
            // index to the variable corresponding player player_ids[j]
            int idx_player = this->player_to_idx_[player_ids[j]];
            team_upper_bound->SetCoefficient(this->player_vars_[idx_player], -1/num_player_ids);
            if (players_from_same_team_constraint != NULL)
            {
                players_from_same_team_constraint->SetCoefficient(this->player_vars_[idx_player], 1);
            }

            team_lower_bound->SetCoefficient(this->player_vars_[idx_player], -1);

            if (j < num_defender_ids)
            {
                int idx_defender = this->player_to_idx_[defender_ids[j]];
                defender_constraint->SetCoefficient(this->player_vars_[idx_defender], 1);
            }
        }
    }// ====================== team constraints =========================

    // =========== number of player constraints in each position ========
    map< string, vector<int> > position_players = this->data_->getPositionPlayers ();

    for (map< string, vector<int> >::iterator it = position_players.begin(); it != position_players.end(); ++it)
    {
        int limit_players = lconstraints->position_players[it->first];
        MPConstraint* position_constraint = this->solver_->MakeRowConstraint(limit_players, limit_players);
        this->position_players_constraints_.push_back (position_constraint);
        vector<int> player_ids = it->second;
        int num_players = player_ids.size ();

        for (int j = 0; j < num_players; ++j)
        {
            int idx_player = this->player_to_idx_[player_ids[j]];
            position_constraint->SetCoefficient(this->player_vars_[idx_player], 1);
        }
    }// =========== number of player constraints in each position ========
}

void Model::createObjective ()
{
    MPObjective* const objective = this->solver_->MutableObjective();
    if (this->sense_ == MAXIMIZE)
    {
        objective->SetMaximization();
    }
    vector<double> scores = this->data_->getAllScores ();

    for (int i = 0; i < this->num_players_; ++i)
    {
        objective->SetCoefficient(this->player_vars_[i], scores[i]);
    }
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

void Model::optimize ()
{
   // cout << "searching solution..." << endl;
    // this->solver_->EnableOutput();
    // cout << this->solver_->SolverVersion() << endl;

    // cout << this->solver_->NumVariables() << endl;
    // cout << this->num_players_ + this->num_teams_ << endl;
    // string name = "";
    // if (this->solver_->ExportModelAsLpFormat(true, &name))
    // {
    //     cout << "foi!" << endl;
    //     cout << name << endl;
    // }
    // else
    //     cout << "falhou" << endl;
    const MPSolver::ResultStatus result_status_ = this->solver_->Solve();


    if (result_status_ != MPSolver::OPTIMAL) {
        LOG(FATAL) << "The problem does not have an optimal solution!";
    }

    // LOG(INFO) << "Optimal objective value = " << this->solver_->MutableObjective()->Value();

    this->solve_time_.push_back (this->solver_->wall_time() - this->solve_time_.back());
    // LOG(INFO) << "Problem solved in " << this->solver_->wall_time() << " milliseconds";
    this->selected_players_.clear();
    vector<Player> players = this->data_->getPlayers();
    for (int i = 0; i < this->num_players_; ++i)
    {
        if (this->player_vars_[i]->solution_value())
        {
            pair<int, string> player_position;
            player_position = make_pair(players[i].getId(), players[i].getPosition());
            this->selected_players_.push_back(player_position);
            // LOG(INFO) << i << " " << this->player_vars_[i]->name() << " = " << this->player_vars_[i]->solution_value();
        }

    }
    // LOG(INFO) << "Solution is  " << this->solver_->VerifySolution(-0.00001, true);
}

vector< pair<int, string> > Model::getSolution()
{
    return this->selected_players_;
    // vector<Player> selected_players;
    // vector<Player> all_players = this->data_->getPlayers();
    // int num_players = this->selected_players_idx_.size();
    // for (int i = 0; i < num_players; ++i)
    // {
    //     int idx = this->selected_players_idx_[i];
    //     selected_players.push_back (all_players[idx]);
    //     // LOG(INFO) << i << " " << this->player_vars_[idx]->name() << " = " << this->player_vars_[idx]->solution_value();
    // }
    // return selected_players;
}

void Model::reoptimize()
{
    vector<double> scores = this->data_->getAllScores ();
    double current_solution = this->solver_->MutableObjective()->Value();
    const double infinity = this->solver_->infinity();
    LineupConstraints *lconstraints = this->data_->getConstraints ();


    MPConstraint* const new_constraint = this->solver_->MakeRowConstraint(-infinity, lconstraints->num_players-1);
    this->lineup_constraints_.push_back (new_constraint);

    int num_players = this->selected_players_.size();
    for (int i = 0; i < num_players; ++i)
    {
        int idx = this->player_to_idx_[this->selected_players_[i].first];
        new_constraint->SetCoefficient(this->player_vars_[idx], 1);
    }


    // MPConstraint* const new_constraint = this->solver_->MakeRowConstraint(-infinity, current_solution - EPSILON);
    //
    // for (int i = 0; i < this->num_players_; ++i)
    // {
    //     new_constraint->SetCoefficient(this->player_vars_[i], scores[i]);
    // }

    this->optimize();
}

void Model::print_times()
{
    for (int i = 0; i < this->solve_time_.size(); ++i)
    {
        cout << this->solve_time_[i] << endl;
    }
}
