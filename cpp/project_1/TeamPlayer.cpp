#include "TeamPlayer.h"

const std::string TeamPlayer::FILE_PATH = "team_player.data";

TeamPlayer::TeamPlayer(Team team, Player player)
    : team(team), player(player) {}

TeamPlayer::~TeamPlayer() {}

std::vector<std::string> TeamPlayer::getArgs() const
{
    return {team.getID(), player.getID()};
}

std::string TeamPlayer::getPath() const
{
    return FILE_PATH;
}

void TeamPlayer::save() const
{
    if (!isKeyUnique(player.getID()))
    {
        return;
    }

    StorableUnit::save();
}

std::vector<Player> TeamPlayer::getPlayersOfTeam(Team team)
{
    std::vector<Player> players;

    for (std::vector<std::string> args : getAllStored(FILE_PATH))
    {
        if (args[0] == team.getID())
        {
            players.push_back(Player::idToPlayer(args[1]));
        }
    }

    return players;
}

void TeamPlayer::removeTeamOrPlayerWithID(std::string ID)
{
    deleteOnAnyMatch(FILE_PATH, ID);
}