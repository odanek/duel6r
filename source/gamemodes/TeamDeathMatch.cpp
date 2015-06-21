//
// Created by marcellus on 16.6.2015.
//

#include "TeamDeathMatch.h"

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

bool Duel6::TeamDeathMatch::roundIsOver(Duel6::World world)
{
    return false;
}

void Duel6::TeamDeathMatch::initialize(Duel6::World *world)
{
    if (eventListener)
    {
        free(eventListener);
    }

    eventListener = (PlayerEventListener *) new TDMPlayerEventListener(&world->getMessageQueue(), friendlyFire);
}

bool Duel6::TDMPlayerEventListener::onDamageByShot(Duel6::Player &player, Duel6::Player &shootingPlayer, Duel6::Float32 amount, Duel6::Shot &shot, bool directHit)
{

    if (!friendlyFire && player.hasTeam(shootingPlayer.getTeam()))
    {
        return false;
    }

    return PlayerEventListener::onDamageByShot(player, shootingPlayer, amount, shot, directHit);
}

void Duel6::TDMPlayerEventListener::onKillByPlayer(Duel6::Player &player, Duel6::Player &killer, Duel6::Shot &shot, bool suicide)
{
    if (!suicide && player.hasTeam(killer.getTeam()))
    {
        messageQueue->add(killer, Format("Killed teammate [{0}]") << player.getPerson().getName());
        killer.getPerson().addPenalties(1);
    }
    else
    {
        PlayerEventListener::onKillByPlayer(player, killer, shot, suicide);
    }
}
