#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

/// @brief Class representing a soccer team
class Team
{
private:
    static const string FILE_PATH;
    int teamID;
    string name;
    string address;
    string phoneNumber;
    string director;

    /// @brief Converts a string to a Team object
    /// @param line String to convert
    /// @return Returns the converted Team object
    static Team fromString(string line)
    {
        string word;
        stringstream ss(line);
        vector<string> result;

        while (getline(ss, word, ' '))
        {
            result.push_back(word);
        }

        return Team(stoi(result[0]), result[1], result[2], result[3], result[4]);
    }

    static void saveTeam(Team team)
    {
        ofstream file(FILE_PATH, ios::app);

        file << team.teamID << " " << team.name << " " << team.address << " " << team.phoneNumber << " " << team.director << endl;

        file.close();
    }

public:
    /// @brief Creates a new Team object
    /// @param teamID ID of the team
    /// @param name Name of the team
    /// @param address Adress of the team
    /// @param phoneNumber Phone number of the team
    /// @param director Current director of the team
    Team(int teamID, string name, string address, string phoneNumber, string director)
        : teamID(teamID), name(name), address(address), phoneNumber(phoneNumber), director(director)
    {
    }
    ~Team() {}

    /// @brief Finds a team by its ID
    /// @param teamID ID of the team to find
    /// @return Returns the team with the given ID or throws an error if it doesn't exist
    static Team idToTeam(int teamID)
    {
        string line;

        ifstream file(FILE_PATH, ios::app);

        while (getline(file, line))
        {
            if (line.substr(0, line.find(" ")) == to_string(teamID))
            {
                return fromString(line);
            }
        }

        file.close();

        throw "Team not found";
    };

    // @todo It's not working
    static void deleteTeam(int teamID)
    {
        string line;

        vector<string> lines;

        ifstream rfile(FILE_PATH);

        while (getline(rfile, line))
        {
            if (line.substr(0, line.find(" ")) == to_string(teamID))
            {
                continue;
            }
            lines.push_back(line);
        }

        rfile.close();

        ofstream wfile(FILE_PATH);

        for (string line : lines)
        {
            wfile << line << endl;
        }

        wfile.close();

        cout << "Team (" + to_string(teamID) + ") deleted" << endl;
    }

    static vector<Team> getAllTeams()
    {
        vector<Team> teams;
        string line;

        ifstream file(FILE_PATH, ios::app);

        while (getline(file, line))
        {
            teams.push_back(fromString(line));
        }

        file.close();

        return teams;
    };

    /// @brief Creates a new team with a new ID
    /// @param name Name of the team
    /// @param address Adress of the team
    /// @param phoneNumber Phone number of the team
    /// @param director Current director of the team
    static Team createTeam(string name, string address, string phoneNumber, string director)
    {
        // @todo Create UUIDs instead of giving random numbers
        Team team(rand(), name, address, phoneNumber, director);

        saveTeam(team);

        cout << "Team created: " << team.teamID << " " << team.name << " " << team.address << " " << team.phoneNumber << " " << team.director << endl;

        return team;
    }

    string getName()
    {
        return name;
    }

    int getID()
    {
        return teamID;
    }

    // vector<Player> getPlayers()
    // {
    //     return Player::getPlayersIn(teamID);
    // }
};

const string Team::FILE_PATH = "teams.data";
