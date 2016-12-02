#include "include/json.hpp"
#include "player.h"
#include "model.h"

#include <iostream>
#include <fstream>
#include <vector>

using json = nlohmann::json;
using namespace std;

int main () {
    json data;
    ifstream myfile ("projections.json");
    vector<Player> players;

    myfile >> data;
    int num_players = data["players"].size ();

    for (int i = 0; i < num_players; ++i)
    {
        long double score =  data["players"][i]["projections"]["fanduel"]["score"];
        long double value = data["players"][i]["projections"]["fanduel"]["score"];
        double salary = data["players"][i]["projections"]["fanduel"]["salary"];
        int production_rating = data["players"][i]["projections"]["fanduel"]["production_rating"];
        int matchup_rating = data["players"][i]["projections"]["fanduel"]["matchup_rating"];
        long double fppg = data["players"][i]["projections"]["fanduel"]["fppg"];
        long double ownership = data["players"][i]["projections"]["fanduel"]["ownership"];

        Projection *fanduel = new Projection (
            score,
            value,
            salary,
            production_rating,
            matchup_rating,
            fppg,
            ownership,
            data["players"][i]["projections"]["fanduel"]["opponent_team"]
        );

        string fanduel_id = data["players"][i]["fanduel_id"];

        Player player(
            stol (fanduel_id),
            data["players"][i]["first_name"],
            data["players"][i]["last_name"],
            data["players"][i]["position"],
            data["players"][i]["team"],
            fanduel
        );

        players.push_back (player);
    }

    cout << "Existem ";
    cout << players.size () << " jogadores." << endl;

    LineupConstraints lconstraints = {};
    lconstraints.min_salary_sum = 59000;
    lconstraints.max_salary_sum = 60000;
    lconstraints.ownership_sum = 105.68;
    lconstraints.min_distinct_teams = 3;
    lconstraints.max_players_from_same_team = 4;

    cout << "criou lconstraints ..." << endl;

    Data* player_data = new Data (players, lconstraints);
    Model* lineup_model = new Model(player_data, MAXIMIZE, "lineup_model");
    lineup_model->createModel();
    // for (int i = 0; i < 500; ++i)
    // {
    //     lineup_model->saveModel();
    //     lineup_model->optimize();
    //     lineup_model->getSolution();
    // }
    // lineup_model->saveModel();
    // lineup_model->optimize(50);
    // lineup_model->getSolution();



    return 0;
}
