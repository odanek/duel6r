//
// Created by marcellus on 16.6.2015.
//

#ifndef DUEL6R_TEAMDEATHMATCH_H
#define DUEL6R_TEAMDEATHMATCH_H

#include "../GameMode.h"

namespace Duel6 {

    class PlayerEventListener;

    class TeamDeathMatch : public GameMode
    {

    private:
        Int32 teamsCount;
        bool friendlyFire;
        PlayerEventListener *eventListener;


    public:
        TeamDeathMatch(Int32 teamsCount, bool friendlyFire)
                : teamsCount(teamsCount), friendlyFire(friendlyFire), eventListener(nullptr)
        { }

        std::string getName()
        {
            return Format("Team deathmatch ({0} teams{1})") << std::to_string(teamsCount) << (friendlyFire ? ", FF" : "");
        }

        void initialize(World *world, Duel6::Game *game);
        void preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &allPlayers);
        bool checkRoundOver(World *world, std::vector<Player*> &alivePlayers);
    };

}

#endif //DUEL6R_TEAMDEATHMATCH_H
