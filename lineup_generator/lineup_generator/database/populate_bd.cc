#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <pqxx/pqxx>
#include "include/json.hpp"

using namespace std;
using namespace pqxx;
using json = nlohmann::json;

int main(int argc, char* argv[])
{
    try
    {
        connection C("dbname=test_db user=evellyn password=qwerty123 \
        hostaddr=127.0.0.1 port=5432");
        if (C.is_open())
        {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } else
        {
            cout << "Can't open database" << endl;
            exit(1);
        }

        // connection successfully opened
        json data;
        ifstream myfile ("projections.json");

        if (!myfile.is_open()) {
            cerr << "Unable to open file" << endl;
            exit(1);
        }

        stringstream insert_sql_command;
        insert_sql_command << "";
        vector<string> players;
        vector<string> projections;

        myfile >> data;
        int num_players = data["players"].size ();

        string fanduel_id;
        string first_name;
        string last_name;
        string position;
        string team;
        string tag;
        string opponent_team;
        string slate;

        set<string> tags;
        set<string> slates;
        pair<set<string>::iterator,bool> result;

        for (int i = 0; i < num_players; ++i)
        {
            fanduel_id = data["players"][i]["fanduel_id"];
            first_name = data["players"][i]["first_name"];
            last_name = data["players"][i]["last_name"];
            position = data["players"][i]["position"];
            team = data["players"][i]["team"];
            //  inserting player i
            insert_sql_command << "INSERT INTO players (fanduel_id, first_name, last_name, position, team) VALUES (" << fanduel_id << ", '" << first_name << "','" << last_name << "','" << position << "','" << team << "');\n";

            // inserting tags for player i
            int num_tags = data["players"][i]["tags"].size ();
            for (int j = 0; j < num_tags; ++j)
            {
                tag = data["players"][i]["tags"][j];
                result = tags.insert(tag);
                if (result.second == true)
                {
                    insert_sql_command << "INSERT INTO tags VALUES ('" << tag << "');\n";
                }
                insert_sql_command << "INSERT INTO players_tags VALUES (" << fanduel_id << ",'" << tag << "');\n";
            }

            //  inserting projection for player i
            opponent_team = data["players"][i]["projections"]["fanduel"]["opponent_team"];

            insert_sql_command << "INSERT INTO fanduel_projections (id, score, value, salary, production_rating, matchup_rating, fppg, ownership, opponent_team) VALUES (" << fanduel_id << "," << data["players"][i]["projections"]["fanduel"]["score"] << "," << data["players"][i]["projections"]["fanduel"]["value"] << "," << data["players"][i]["projections"]["fanduel"]["salary"] << "," << data["players"][i]["projections"]["fanduel"]["production_rating"] << ",";
            insert_sql_command << data["players"][i]["projections"]["fanduel"]["matchup_rating"] << "," << data["players"][i]["projections"]["fanduel"]["fppg"] << "," << data["players"][i]["projections"]["fanduel"]["ownership"] << ",'" << team << "');\n";

            //  inserting slates for projection of player i
            int num_slates = data["players"][i]["projections"]["fanduel"]["slates"].size ();
            for (int j = 0; j < num_slates; ++j)
            {
                slate = data["players"][i]["projections"]["fanduel"]["slates"][j];
                result = slates.insert(slate);
                if (result.second == true)
                {
                    insert_sql_command << "INSERT INTO slates VALUES ('" << slate << "');\n";
                }
                insert_sql_command << "INSERT INTO fanduel_slates VALUES (" << fanduel_id << ",'" << slate << "');\n";
            }

        }// for each player
        /* Create a transactional object. */
        work W(C);

        /* Execute SQL query */
        W.exec(insert_sql_command.str());
        W.commit();
        //
        cout << insert_sql_command.str();
        cout << "Records created successfully" << endl;
        C.disconnect ();

    } catch (const std::exception &e)
    {
        cerr << e.what() << std::endl;
        exit(1);
    }
}
