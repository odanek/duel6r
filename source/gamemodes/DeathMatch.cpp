//
// Created by marcellus on 16.6.2015.
//

#include "DeathMatch.h"
#include "../Game.h"

void Duel6::DeathMatch::preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &players)
{
    player->unsetTeam();
    player->unsetOverlay();
    player->setEventListener(eventListener);
}


bool Duel6::DeathMatch::checkRoundOver(Duel6::World *world, std::vector<Duel6::Player*> &alivePlayers)
{
    if (alivePlayers.size() == 1)
    {
        for (Player *player : alivePlayers)
        {
            world->getMessageQueue().add(*player, D6_L("You have won!"));
            player->getPerson().addWins(1);
        }
        return true;
    }
    else if (alivePlayers.size() == 0)
    {
        for (const Player& player : world->getPlayers())
        {
            world->getMessageQueue().add(player, D6_L("End of round - no winner"));
        }
        return true;
    }
    return false;
}


void Duel6::DeathMatch::initialize(Duel6::World *world, Duel6::Game *game)
{
    if (eventListener)
    {
        delete(eventListener);
    }

    eventListener = new PlayerEventListener(&world->getMessageQueue(), &game->getSettings());
}
