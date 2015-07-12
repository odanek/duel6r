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
    class PlayerEventListener;

    class DeathMatch : public GameMode
    {

    private:
        PlayerEventListener* eventListener;


    public:
        DeathMatch()
            :eventListener(nullptr)
        {};

        std::string getName()
        {
            return "Deathmatch";
        }
        void initialize(World *world, Duel6::Game *game);
        void preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &allPlayers);
        bool checkRoundOver(World *world, std::vector<Player*> &alivePlayers);
    };
}


#endif //DUEL6R_DEATHMATCH_H
