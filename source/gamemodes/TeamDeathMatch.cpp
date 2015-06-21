//
// Created by marcellus on 16.6.2015.
//

#include "TeamDeathMatch.h"
#include "../Game.h"
#include "TeamDeathMatchPlayerEventListener.h"

// Predefined teams as a temporary solution
std::vector<std::string> TEAMS = {"Alpha", "Bravo", "Charlie", "Delta", "Echo"};
std::vector<Duel6::Color> COLORS = { Duel6::Color(255, 0, 0), Duel6::Color(0, 255, 0), Duel6::Color(0, 0, 255), Duel6::Color(219, 216, 241), Duel6::Color(115,137,186)};


void Duel6::TeamDeathMatch::preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &allPlayers)
{
    int teamSize = std::max(0, (int)allPlayers.size() / teamsCount);
    int playerTeam = std::min(playerIndex / teamSize, teamsCount - 1);

    // todo: add support for custom teams in future versions
    std::string& teamName = TEAMS.at(playerTeam);
    Color teamColor = COLORS.at(playerTeam);

    player->setTeam(teamName);
    player->setOverlay(teamColor);
    player->setEventListener(eventListener);
}


bool Duel6::TeamDeathMatch::checkRoundOver(Duel6::World *world, std::vector<Duel6::Player*> &alivePlayers)
{
    bool oneTeamRemaining = true;
    std::string lastAliveTeam = "";

    for (Player *player : alivePlayers)
    {
        if (lastAliveTeam == "")
        {
            lastAliveTeam = player->getTeam();
        }
        else if (lastAliveTeam != player->getTeam())
        {
            oneTeamRemaining = false;
            break;
        }
    }

    if (oneTeamRemaining)
    {
        for(Player& player : world->getPlayers())
        {
            if (player.hasTeam(lastAliveTeam))
            {
                world->getMessageQueue().add(player, Format("Team {0} won!") << lastAliveTeam);
                player.getPerson().addWins(1);
            }
        }
    }

    return oneTeamRemaining;
}


void Duel6::TeamDeathMatch::initialize(Duel6::World *world, Duel6::Game *game)
{
    if (eventListener)
    {
        delete eventListener;
    }

    eventListener = new TeamDeathMatchPlayerEventListener (&world->getMessageQueue(), &game->getSettings(), friendlyFire);
}