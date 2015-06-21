//
// Created by marcellus on 16.6.2015.
//

#ifndef DUEL6R_DEATHMATCH_H
#define DUEL6R_DEATHMATCH_H


#include <stdlib.h>
#include <strings.h>

#include "../GameMode.h"

namespace Duel6
{

    class DeathMatch : public GameMode
    {

    public:
        DeathMatch(){};
        std::string getName()
        {
            return "Deathmatch";
        }
        void initialize(World *world);
        void preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &allPlayers);
        bool roundIsOver(Duel6::World world);
    };
}


#endif //DUEL6R_DEATHMATCH_H
