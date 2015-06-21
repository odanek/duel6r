//
// Created by marcellus on 16.6.2015.
//

#include "DeathMatch.h"


void Duel6::DeathMatch::preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &players)
{
    player->unsetTeam();
    player->unsetOverlay();
}

bool Duel6::DeathMatch::roundIsOver(Duel6::World world)
{
    return false;
}

void Duel6::DeathMatch::initialize(Duel6::World *world)
{

}
