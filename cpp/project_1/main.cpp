#include <iostream>
#include <functional>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;

#include "Match.h"
#include "Player.h"
#include "Team.h"
#include "TeamStats.h"
#include "./util/Menu.h"
#include "./util/MenuContext.h"
#include "./util/Utils.h"
#include "TeamPlayer.h"
#include "PlayerGoal.h"

// Define the description of the controls
#ifdef _WIN32
#define CONTROL_DESCRIPTION "Use the W and S keys to navigate between options,\nEnter to select an option and Backspace to go back."
#else
#define CONTROL_DESCRIPTION "Use the Up and Down arrow keys to navigate between options,\nEnter to select an option and Left arrow key to go back."
#endif

// Forward declarations
Menu mainMenu();
Menu teamMenu();
Menu manageTeamMenu(Team);
Menu teamListMenu(function<void(MenuContext, Team)>, string, string = "");
Team createTeam();
Player createPlayer();
Menu addPlayerMenu(Team);
Menu managePlayerMenu(Player);
Menu managePlayersOfTeam(Team, function<void(MenuContext, Player)>, string, string = "");
Menu playerMenu();
Menu playersListMenu(function<void(MenuContext, Player)>, string, string = "");
Menu manageTeamPlayerMenu(Team, Player);
Menu managePlayerTeamMenu(Player, Team);
Menu manageGameRecordsMenu();
Menu manageMatchMenu(Match);
Menu matchListMenu();
Match createMatch();
Menu updatePlayerMenu(Player);
void updatePlayerName(Player);
Menu updateTeamMenu(Team);
Menu statsOfTeamsMenu(DateConstraint);
Menu updateMatchMenu(Match);
Menu updateStatsMenu(TeamStats);
Menu showTeamStatsMenu();
Menu getPositionMenu(function<void(MenuContext, PlayingPosition)>, string, string = "");
Menu manageGoalsMenu(TeamStats);
Menu goalListMenu(TeamStats);

int main()
{
    //  Seed the random number generator
    srand((unsigned int)time(NULL));

    // Start the main menu
    MenuContext::run(mainMenu());

    return 0;
}

Menu mainMenu()
{
    return Menu({
                    MenuOption{
                        "Team Menu",
                        [](MenuContext context)
                        {
                            context.push(teamMenu());
                        },
                    },
                    MenuOption{
                        "Player Menu",
                        [](MenuContext context)
                        {
                            context.push(playerMenu());
                        },

                    },
                    MenuOption{
                        "Game Records Menu",
                        [](MenuContext context)
                        {
                            context.push(manageGameRecordsMenu());
                        },
                    },
                },
                "Main Menu", CONTROL_DESCRIPTION);
}

Menu teamMenu()
{
    return Menu({
                    {
                        "Create Team",
                        [](MenuContext context)
                        {
                            Team team = createTeam();
                            context.push([team]()
                                         { return manageTeamMenu(team); });
                        },
                    },
                    {
                        "Show Teams",
                        [](MenuContext context)
                        {
                            context.push([]()
                                         { return teamListMenu(
                                               [](MenuContext context, Team team)
                                               { context.push([team]()
                                                              { return manageTeamMenu(team); }); },
                                               "Showing Teams", "You can manage, or learn more about a team by selecting them."); });
                        },
                    },
                },
                "Teams", "You can create a team, or manage an existing one by selecting them.\nTo see the list of all teams, select the \"Show Teams\" option.");
}

Menu teamListMenu(function<void(MenuContext, Team)> callback, string title, string description)
{
    vector<MenuOption> options = {};

    for (Team team : Team::getAllTeams())
    {
        options.push_back(MenuOption{
            team.getName(),
            [team, callback](MenuContext context)
            {
                callback(context, team);
            },
        });
    }

    return Menu(options, title + " (Found " + to_string(options.size()) + " teams)", description);
}

Menu managePlayersOfTeam(Team team, function<void(MenuContext, Player)> callback, string title, string description)
{
    vector<MenuOption> options = {};

    for (Player player : team.getPlayers())
    {
        options.push_back(MenuOption{
            player.getName() + " " + player.getSurname() + " (" + Player::positionToString(player.getPosition()) + ")",
            [callback, player](MenuContext context)
            {
                callback(context, player);
            },
        });
    }

    return Menu(options, title + " (Found " + to_string(options.size()) + " players)", description);
}

Menu manageTeamPlayerMenu(Team team, Player player)
{
    // In order the menu to be updated, we need to access the team and player objects from the file
    team = Team::idToTeam(team.getID());
    player = Player::idToPlayer(player.getID());

    return Menu({
                    {
                        "Show Player",
                        [player](MenuContext context)
                        {
                            context.push([player]()
                                         { return managePlayerMenu(player); });
                        },
                    },
                    {
                        "Player Position: " + Player::positionToString(player.getPosition()),
                        [player](MenuContext context) mutable
                        {
                            context.push(getPositionMenu(
                                [player](MenuContext context, PlayingPosition pos) mutable
                                {
                                    player.setPosition(pos);
                                    context.pop();
                                },
                                "Playing Positions", "Select the position of the player. This will change the position detail of the player."));
                        },
                    },
                    {
                        "Remove from " + team.getName() + " (Team)",
                        [ID = player.getID()](MenuContext context)
                        {
                            TeamPlayer::removeTeamOrPlayerWithID(ID);
                            context.pop();
                        },
                    },
                },
                "Managing " + player.getName() + " " + player.getSurname() + " (Player) from " + team.getName() + " (Team)", "Manage the player or remove them from the team.");
}

Menu managePlayerTeamMenu(Player player, Team team)
{
    // In order the menu to be updated, we need to access the team and player objects from the file
    player = Player::idToPlayer(player.getID());
    team = Team::idToTeam(team.getID());

    return Menu({
                    {
                        "Show Team",
                        [team](MenuContext context)
                        {
                            context.push([team]()
                                         { return manageTeamMenu(team); });
                        },
                    },
                    {
                        "Remove from " + team.getName() + " (Team)",
                        [ID = player.getID()](MenuContext context)
                        {
                            TeamPlayer::removeTeamOrPlayerWithID(ID);
                            context.pop();
                        },
                    },
                },
                "Managing " + player.getName() + " " + player.getSurname() + " (Player) in " + team.getName() + " (Team)", "Show the team or remove the player from the team.");
}

Menu manageTeamMenu(Team team)
{
    // In order the menu to be updated, we need to access the team object from the file
    team = Team::idToTeam(team.getID());

    return Menu({
                    {
                        "Add Player",
                        [team](MenuContext context)
                        {
                            context.push([team]()
                                         { return addPlayerMenu(team); });
                        },
                    },
                    {
                        "Show Players",
                        [team](MenuContext context)
                        {
                            context.push([team]()
                                         { return managePlayersOfTeam(
                                               team, [team](MenuContext context, Player player)
                                               { context.push([team, player]()
                                                              { return manageTeamPlayerMenu(team, player); }); },
                                               "Showing the Players of " + team.getName() + " (Team)", "Select a player to manage."); });
                        },
                    },
                    {
                        "Show Details",
                        [team](MenuContext context)
                        {
                            context.push([team]()
                                         { return updateTeamMenu(team); });
                        },
                    },
                    {
                        "Delete Team",
                        [team](MenuContext context)
                        {
                            team.deleteTeam();
                            context.pop();
                        },
                    },
                },
                "Showing " + team.getName() + " (Team)", "Manage or learn more about the team.");
}

Menu playerMenu()
{

    return Menu({
                    {
                        "Create Player",
                        [](MenuContext context)
                        {
                            Player player = createPlayer();
                            context.push([player]()
                                         { return managePlayerMenu(player); });
                        },
                    },
                    {
                        "Show Players",
                        [](MenuContext context)
                        {
                            context.push([]()
                                         { return playersListMenu(
                                               [](MenuContext context, Player player)
                                               { context.push([player]()
                                                              { return managePlayerMenu(player); }); },
                                               "Showing Players", "You can manage, or learn more about a player by selecting them."); });
                        },
                    },
                },
                "Player Menu", "You can create a player, or manage an existing one by selecting them.\nTo see the list of all players, select the \"Show Players\" option.");
}

Menu playersListMenu(function<void(MenuContext, Player)> callback, string title, string description)
{
    vector<MenuOption> options = {};

    for (Player player : Player::getAllPlayers())
    {
        options.push_back(MenuOption{
            player.getName() + " " + player.getSurname(),
            [player, callback](MenuContext context)
            {
                callback(context, player);
            },
        });
    }

    return Menu(options, title + " (Found " + to_string(options.size()) + " players)", description);
}

Menu addPlayerMenu(Team team)
{
    vector<MenuOption> options = {};

    for (Player player : Player::getAllPlayers())
    {
        std::optional<Team> teamOfPlayer = TeamPlayer::getTeamOfPlayer(player);

        if (teamOfPlayer.has_value())
        {
            options.push_back(MenuOption{
                player.getName() + " " + player.getSurname() + " (in " + teamOfPlayer.value().getName() + ")",
            });
        }
        else
        {
            options.push_back(MenuOption{
                player.getName() + " " + player.getSurname(),
                [player, team](MenuContext context)
                {
                    team.addPlayer(player);
                    context.pop();
                },
            });
        }
    }

    return Menu(options, "Add a Player to " + team.getName() + " (Team) (Found " + to_string(options.size()) + " players)", "Select a player to add to the team. A player can only be in one team at a time.");
}

Menu managePlayerMenu(Player player)
{
    // In order the menu to be updated, we need to access the player object from the file
    player = Player::idToPlayer(player.getID());

    return Menu({
                    {
                        "Show Details",
                        [player](MenuContext context)
                        {
                            context.push([player]()
                                         { return updatePlayerMenu(player); });
                        },
                    },
                    {
                        "Delete Player",
                        [player](MenuContext context)
                        {
                            player.deletePlayer();
                            context.pop();
                        },
                    },
                },
                "Showing " + player.getName() + " " + player.getSurname() + " (Player)", "Manage or learn more about the player.");
}

Menu manageGameRecordsMenu()
{
    return Menu({
                    {
                        "Create Game Record",
                        [](MenuContext context)
                        {
                            Match match = createMatch();
                            context.push([match]()
                                         { return manageMatchMenu(match); });
                        },
                    },
                    {
                        "Show Game Records",
                        [](MenuContext context)
                        {
                            context.push(matchListMenu);
                        },
                    },
                    {
                        "Show Stats Of Teams",
                        [](MenuContext context)
                        {
                            context.push(showTeamStatsMenu);
                        },
                    },
                },
                "Game Records Menu", "You can create a game record, or manage an existing one by selecting them.\nTo see the list of all game records, select the \"Show Game Records\" option.");
}

Menu showTeamStatsMenu()
{
    // @todo Find a way to do it without using static variables
    static DateConstraint constraint;

    return Menu({
                    {
                        "From: " + (constraint.from.has_value() ? Utils::dateToString(constraint.from.value()) : "Not Set"),
                        [](MenuContext context) mutable
                        {
                            Utils::clearScreen();

                            string date;

                            cout << "Enter the date (DD-MM-YYYY): ";
                            cin >> date;

                            constraint.from = Utils::stringToDate(date);

                            cin.ignore();

                            context.reload();
                        },
                    },
                    {
                        "To: " + (constraint.to.has_value() ? Utils::dateToString(constraint.to.value()) : "Not Set"),
                        [](MenuContext context) mutable
                        {
                            Utils::clearScreen();

                            string date;

                            cout << "Enter the date (DD-MM-YYYY): ";
                            cin >> date;

                            constraint.to = Utils::stringToDate(date);

                            cin.ignore();

                            context.reload();
                        },
                    },
                    {
                        "Show Stats",
                        [](MenuContext context)
                        {
                            context.push(statsOfTeamsMenu(constraint));
                        },
                    },
                },
                "Show Stats Of Teams", "You can set a date constraint to show the stats of teams in a specific time period,\nor show the stats of all time by leaving the dates empty.\nTo show the stats with the current date constraint, select the \"Show Stats\" option.");
}

Menu statsOfTeamsMenu(DateConstraint constraint)
{
    int teams = 0;

    stringstream desc("");
    desc << left;

    desc << endl
         << "+" + string(40, '-') + "+" << endl
         << "|" << setw(10) << "Team" << setw(10) << "Goals" << setw(10) << "Wins" << setw(10) << "Losses"
         << "|" << endl
         << "+" + string(40, '-') + "+" << endl;

    for (Team team : Team::getAllTeams())
    {
        int goals = team.getTotalGoals(constraint);
        int wins = team.getTotalWins(constraint);
        int losses = team.getTotalLosses(constraint);

        if (goals == 0 && wins == 0 && losses == 0)
            continue;

        teams++;

        desc << "|" << setw(10) << team.getName() << setw(10) << to_string(goals) << setw(10) << to_string(wins) << setw(10) << to_string(losses) << "|" << endl;
    }

    desc << "+" + string(40, '-') + "+" << endl
         << endl;

    string title = "All Time";

    if (constraint.from.has_value() && constraint.to.has_value())
    {
        title = "From " + Utils::dateToString(constraint.from.value()) + " To " + Utils::dateToString(constraint.to.value());
    }
    else if (constraint.from.has_value())
    {
        title = "From " + Utils::dateToString(constraint.from.value());
    }
    else if (constraint.to.has_value())
    {
        title = "Until " + Utils::dateToString(constraint.to.value());
    }

    return Menu({}, "Showing Stats Of Teams " + title + " (Found " + to_string(teams) + " teams)", desc.str(), false);
}

Menu matchListMenu()
{
    vector<MenuOption> options = {};

    for (Match match : Match::getAllMatches())
    {
        stringstream title("");

        title << left;

        title << setw(10) << Utils::dateToString(match.getDate()) << right << setw(15) << match.getWinner().getTeam().getName() << " vs " << left << setw(15) << match.getLoser().getTeam().getName() << setw(15) << "[" + to_string(match.getWinner().getGoals().size()) + ":" + to_string(match.getLoser().getGoals().size()) + "]";

        options.push_back(MenuOption{
            title.str(),
            [match](MenuContext context)
            {
                context.push([match]()
                             { return manageMatchMenu(match); });
            },
        });
    }

    return Menu(options, "Showing Game Records (Found " + to_string(options.size()) + " records)", "You can manage, or learn more about a game record by selecting them.");
}

Menu manageMatchMenu(Match match)
{
    // In order the menu to be updated, we need to access the match object from the file
    match = Match::idToMatch(match.getID());

    return Menu({
                    {
                        "Show Details",
                        [match](MenuContext context)
                        {
                            context.push([match]()
                                         { return updateMatchMenu(match); });
                        },
                    },
                    {
                        "Delete Game Record",
                        [match](MenuContext context)
                        {
                            match.deleteMatch();
                            context.pop();
                        },
                    },
                },
                "Showing " + Utils::dateToString(match.getDate()) + " " + match.getWinner().getTeam().getName() + " " + match.getLoser().getTeam().getName() + " [" + to_string(match.getWinner().getGoals().size()) + ":" + to_string(match.getLoser().getGoals().size()) + "]" + " (Game Record)", "Manage or learn more about the game record.");
}

Menu manageGoalsMenu(TeamStats stats)
{
    return Menu({
                    {
                        "Add Goal",
                        [stats](MenuContext context) mutable
                        {
                            context.push(managePlayersOfTeam(
                                stats.getTeam(), [stats](MenuContext context, Player player)
                                {
                                    Utils::clearScreen();

                                    int time;
                                    cout << "Enter the time as seconds: ";
                                    cin >> time;

                                    cin.ignore();

                                    stats.addGoal(player, time);
                                    context.pop(); },
                                "Showing the Players of " + stats.getTeam().getName() + " (Team)", "Select a player to add a goal to the team."));
                        },
                    },
                    {
                        "Show Goals (" + to_string(stats.getGoals().size()) + ")",
                        [stats](MenuContext context)
                        {
                            context.push([stats]()
                                         { return goalListMenu(stats); });
                        },
                    },
                },
                "Goals Menu of " + stats.getTeam().getName() + " (Team)", "You can add a goal to the team, or see the list of goals by selecting them.");
}

Menu goalListMenu(TeamStats stats)
{
    vector<MenuOption> options = {};

    stringstream description("");

    description << left;

    description << "You can manage, or learn more about a game record by selecting them." << endl
                << endl
                << "+" + string(53, '-') + "+" << endl
                << "|   " << setw(10) << "Time" << setw(15) << "Team" << setw(25) << "Player"
                << "|" << endl
                << "+" + string(53, '-') + "+" << endl;

    for (PlayerGoal goal : stats.getGoals())
    {
        stringstream title("");

        title << left;

        string timeStr = to_string((int)floor(goal.getTime() / 60)) + ":" + ((goal.getTime() % 60) > 9 ? "" : "0") + to_string(goal.getTime() % 60);

        title << setw(10) << timeStr << setw(15) << goal.getStats().getTeam().getName() << setw(25) << goal.getPlayer().getName() + " " + goal.getPlayer().getSurname();

        options.push_back(MenuOption{
            title.str(),
            // [goal](MenuContext context)
            // {
            //     context.push([match]()
            //                  { return manageMatchMenu(match); });
            // },
        });
    }

    return Menu(options, "Showing Goals of " + stats.getTeam().getName() + " (Found " + to_string(options.size()) + " records)", description.str());
}

// Functional units

// Update functions

Menu updateMatchMenu(Match match)
{
    // In order the menu to be updated, we need to access the match object from the file
    match = Match::idToMatch(match.getID());

    return Menu({
                    {
                        "Date: " + Utils::dateToString(match.getDate()),
                        [match](MenuContext context) mutable
                        {
                            Utils::clearScreen();

                            string date;
                            cout << "Enter the new date (DD-MM-YYYY): ";
                            cin >> date;
                            match.setDate(Utils::stringToDate(date));

                            cin.ignore();

                            context.reload();
                        },
                    },
                    {
                        "Stats of Winner: " + match.getWinner().getTeam().getName() + " team, " + to_string(match.getWinner().getGoals().size()) + " goals",
                        [match](MenuContext context)
                        {
                            context.push(
                                [match]()
                                {
                                    return updateStatsMenu(match.getWinner());
                                });
                        },
                    },
                    {
                        "Stats of Loser: " + match.getLoser().getTeam().getName() + " team, " + to_string(match.getLoser().getGoals().size()) + " goals",
                        [match](MenuContext context)
                        {
                            context.push(
                                [match]()
                                {
                                    return updateStatsMenu(match.getLoser());
                                });
                        },
                    },
                },
                "Showing the details of " + Utils::dateToString(match.getDate()) + " " + match.getWinner().getTeam().getName() + " " + match.getLoser().getTeam().getName() + " [" + to_string(match.getWinner().getGoals().size()) + ":" + to_string(match.getLoser().getGoals().size()) + "]" + " (Game Record)", "Update the details of the game record by selecting the related option.");
}

Menu updateStatsMenu(TeamStats stats)
{
    // In order the menu to be updated, we need to access the stats object from the file
    stats = TeamStats::idToTeamStats(stats.getID());

    return Menu({
                    {
                        "Team: " + stats.getTeam().getName(),
                        [stats](MenuContext context) mutable
                        {
                            context.push(teamListMenu([stats](MenuContext context, Team team) mutable
                                                      { 
                                                        stats.changeTeam(team);
                                                        context.pop(); },
                                                      "Select the new Team", "This will delete all the goals of the team."));
                        },
                    },
                    {
                        "Goals: " + to_string(stats.getGoals().size()),
                        [stats](MenuContext context) mutable
                        {
                            context.push([stats]()
                                         { return manageGoalsMenu(stats); });
                        },
                    },
                },
                "Showing the Stats of the " + stats.getTeam().getName() + " (Team)", "Update the details by selecting the related option.");
}

Menu updatePlayerMenu(Player player)
{
    // In order the menu to be updated, we need to access the player object from the file
    player = Player::idToPlayer(player.getID());

    optional<Team> team = TeamPlayer::getTeamOfPlayer(player);

    return Menu({
                    {
                        "Name: " + player.getName(),
                        [player](MenuContext context) mutable
                        {
                            Utils::clearScreen();

                            string name;
                            cout << "Enter the new name: ";
                            cin >> name;
                            player.setName(name);

                            cin.ignore();

                            context.reload();
                        },
                    },
                    {
                        "Surname: " + player.getSurname(),
                        [player](MenuContext context) mutable
                        {
                            Utils::clearScreen();

                            string surname;
                            cout << "Enter the new surname: ";
                            cin >> surname;
                            player.setSurname(surname);

                            cin.ignore();

                            context.reload();
                        },
                    },
                    {
                        "License ID: " + player.getLicenseID(),
                        [player](MenuContext context) mutable
                        {
                            Utils::clearScreen();

                            string licenseID;
                            cout << "Enter the new license ID: ";
                            cin >> licenseID;
                            player.setLicenseID(licenseID);

                            cin.ignore();

                            context.reload();
                        },
                    },
                    {
                        "Position: " + Player::positionToString(player.getPosition()),
                        [player](MenuContext context) mutable
                        {
                            context.push(getPositionMenu(
                                [player](MenuContext context, PlayingPosition pos) mutable
                                {
                                    player.setPosition(pos);
                                    context.pop();
                                },
                                "Playing Positions", "Select the position of the player."));
                        },
                    },
                    {
                        "Salary: " + to_string(player.getSalary()),
                        [player](MenuContext context) mutable
                        {
                            Utils::clearScreen();

                            int salary;
                            cout << "Enter the new salary: ";
                            cin >> salary;
                            player.setSalary(salary);

                            cin.ignore();

                            context.reload();
                        },
                    },
                    {
                        "Date Of Birth: " + Utils::dateToString(player.getDate()),
                        [player](MenuContext context) mutable
                        {
                            Utils::clearScreen();

                            string date;
                            cout << "Enter the new date of birth (DD-MM-YYYY): ";
                            cin >> date;
                            player.setDate(Utils::stringToDate(date));

                            cin.ignore();

                            context.reload();
                        },
                    },
                    {
                        "Team: " + (team.has_value() ? team.value().getName() : "Not Set"),
                        [team, player](MenuContext context)
                        {
                            if (team.has_value())
                            {
                                context.push([team, player]()
                                             { return managePlayerTeamMenu(player, team.value()); });
                            }
                            else
                            {
                                context.push([player]()
                                             { return teamListMenu(
                                                   [player](MenuContext context, Team team)
                                                   {
                                                       team.addPlayer(player);
                                                       context.pop();
                                                   },
                                                   "Add " + player.getName() + " " + player.getSurname() + " to a Team",
                                                   "Select a team to add the player to."); });
                            }
                        },
                    },
                },
                "Showing the details of " + player.getName() + " " + player.getSurname() + " (Player)", "Update the details of the player by selecting the related option.");
}

Menu updateTeamMenu(Team team)
{
    // In order the menu to be updated, we need to access the player object from the file
    team = Team::idToTeam(team.getID());

    return Menu({{
                     "Name: " + team.getName(),
                     [team](MenuContext context) mutable
                     {
                         Utils::clearScreen();

                         string name;
                         cout << "Enter the new name: ";
                         cin >> name;
                         team.setName(name);

                         cin.ignore();

                         context.reload();
                     },
                 },
                 {
                     "Address: " + team.getAddr(),
                     [team](MenuContext context) mutable
                     {
                         Utils::clearScreen();

                         string address;
                         cout << "Enter the new address: ";
                         cin >> address;
                         team.setAddr(address);

                         cin.ignore();

                         context.reload();
                     },
                 },
                 {
                     "Phone Number: " + team.getPhone(),
                     [team](MenuContext context) mutable
                     {
                         Utils::clearScreen();

                         string phoneNumber;
                         cout << "Enter the new phone number: ";
                         cin >> phoneNumber;
                         team.setPhone(phoneNumber);

                         cin.ignore();

                         context.reload();
                     },
                 },
                 {
                     "Director: " + team.getDirector(),
                     [team](MenuContext context) mutable
                     {
                         Utils::clearScreen();

                         string director;
                         cout << "Enter the new director: ";
                         cin >> director;
                         team.setDirector(director);

                         cin.ignore();

                         context.reload();
                     },
                 }

                },
                "Showing the details of " + team.getName() + " (Team)", "Update the details of the team by selecting the related option.");
}

// Creation functions

Team createTeam()
{
    Utils::clearScreen();

    string name, address, phoneNumber, director;

    cout << "Enter the information of the team:" << endl;

    cout << "Name: ";
    cin >> name;

    cout << "Address: ";
    cin >> address;

    cout << "Phone Number: ";
    cin >> phoneNumber;

    cout << "Director: ";
    cin >> director;

    cin.ignore();

    return Team::createTeam(name, address, phoneNumber, director);
}

Player createPlayer()
{
    Utils::clearScreen();

    string name, surname, licenseID, dateOfBirth;
    int salary;
    PlayingPosition *position;

    cout << "Enter the information of the player:" << endl;

    cout << "Name: ";
    cin >> name;

    cout << "Surname: ";
    cin >> surname;

    cin.ignore();

    MenuContext::run(
        getPositionMenu(
            [position](MenuContext context, PlayingPosition pos) mutable
            {
                *position = pos;
                context.pop();
            },
            "Playing Positions", "Select the position of the player."),
        false);

    cout << "License ID: ";
    cin >> licenseID;

    cout << "Salary: ";
    cin >> salary;

    cout << "Date of Birth (DD-MM-YYYY): ";
    cin >> dateOfBirth;

    cin.ignore();

    return Player::createPlayer(name, surname, licenseID, *position, salary, Utils::stringToDate(dateOfBirth));
}

Match createMatch()
{
    Utils::clearScreen();

    string date;

    cout << "Enter the date of the match (DD-MM-YYYY): ";
    cin >> date;

    Match match = Match(Utils::stringToDate(date));

    cin.ignore();

    // Get the teams

    vector<Team> teams;
    teams.reserve(2);

    for (int i = 0; i < 2; i++)
    {
        MenuContext::run(teamListMenu([&teams](MenuContext context, Team team) mutable
                                      { teams.push_back(team); },
                                      "Select the #" + to_string(i + 1) + " Team"),
                         false);
    }

    // Create the match

    match.create(teams);

    return match;
}

// Functional

Menu getPositionMenu(function<void(MenuContext, PlayingPosition)> callback, string title, string description)
{
    return Menu(
        {
            {"Keeper", [callback](MenuContext context) mutable
             {
                 callback(context, PlayingPosition::keeper);
             }},
            {"Defender", [callback](MenuContext context) mutable
             {
                 callback(context, PlayingPosition::defender);
             }},
            {"Midfielder", [callback](MenuContext context) mutable
             {
                 callback(context, PlayingPosition::midfielder);
             }},
            {"Forward", [callback](MenuContext context) mutable
             {
                 callback(context, PlayingPosition::forward);
             }},
        },
        title, description);
}