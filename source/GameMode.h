//
// Created by marcellus on 16.6.2015.
//

#ifndef DUEL6R_GAMEMODE_H
#define DUEL6R_GAMEMODE_H

#include <string>
#include <vector>
#include "Player.h"
#include "World.h"

namespace Duel6
{
    class GameMode
    {
    public:
        virtual std::string getName() = 0;
        virtual void initialize(World *world) = 0;
        virtual void preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &allPlayers) = 0;
        virtual bool roundIsOver(World world) = 0;
    };
}
#endif //DUEL6R_GAMEMODE_H
