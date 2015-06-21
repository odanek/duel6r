//
// Created by marcellus on 16.6.2015.
//

#ifndef DUEL6R_TEAMDEATHMATCH_H
#define DUEL6R_TEAMDEATHMATCH_H

#include "../GameMode.h"

namespace Duel6 {

    class TDMPlayerEventListener;
    class PlayerEventListener;

    class TeamDeathMatch : public GameMode {
    private:
        Int32 teamsCount;
        bool friendlyFire;
        Int32 nextTeam;
        PlayerEventListener* eventListener;
    public:
        TeamDeathMatch(Int32 teamsCount, bool friendlyFire)
                : teamsCount(teamsCount), friendlyFire(friendlyFire)
        { }

        std::string getName()
        {
            return Format("Team deathmatch ({0} teams{1})") << std::to_string(teamsCount) << (friendlyFire? ", FF" : "");
        }

        void initialize(World *world);
        void preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &allPlayers);
        bool roundIsOver(Duel6::World world);
    };


    class TDMPlayerEventListener : PlayerEventListener
    {
    private:
        bool friendlyFire;
    public:
        TDMPlayerEventListener(InfoMessageQueue *messageQueue, bool friendlyFire)
                : PlayerEventListener(messageQueue), friendlyFire(friendlyFire)
        {

        }

        bool onDamageByShot(Duel6::Player &player, Duel6::Player &shootingPlayer, Duel6::Float32 amount, Duel6::Shot &shot, bool directHit);

        void onKillByPlayer(Player &player, Player &killer, Shot &shot, bool suicice);
    };

}

#endif //DUEL6R_TEAMDEATHMATCH_H
