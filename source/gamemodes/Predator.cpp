//
// Created by marcellus on 21.6.2015.
//

#include "Predator.h"
#include "../Game.h"
#include "PredatorPlayerEventListener.h"

void Duel6::Predator::initialize(Duel6::World *world, Duel6::Game *game)
{
    predatorIndex = (int) (rand() % world->getPlayers().size());

    if (eventListener)
    {
        delete(eventListener);
    }

    eventListener = new PredatorPlayerEventListener(&world->getMessageQueue(), &game->getSettings());
}

void Duel6::Predator::preparePlayer(Duel6::Player *player, Duel6::Int32 playerIndex, std::vector<Duel6::Player> &allPlayers)
{
    if (playerIndex == predatorIndex)
    {
        player->setTeam(PREDATOR_TEAM());
        player->setOverlay(Color(10,0,0,0));
    }
    else
    {
        player->setTeam(HUNTERS_TEAM());
        player->unsetOverlay();
        player->pickAmmo(10);
    }

    player->setEventListener(eventListener);
}

bool Duel6::Predator::checkRoundOver(Duel6::World *world, std::vector<Duel6::Player*> &alivePlayers)
{

    if (alivePlayers.size() == 0)
    {
        for (const Player& player : world->getPlayers())
        {
            world->getMessageQueue().add(player, D6_L("End of round - no winner"));
        }
        return true;
    }


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
                if (lastAliveTeam == HUNTERS_TEAM())
                {
                    if (!player.isDead())
                    {
                        world->getMessageQueue().add(player, Format("Marines won!"));
                        player.getPerson().addWins(1);
                    }
                }
                else
                {
                    world->getMessageQueue().add(player, Format("Predator won!"));
                    player.getPerson().addWins(1);
                }
            }
        }
    }

    return oneTeamRemaining;
}

