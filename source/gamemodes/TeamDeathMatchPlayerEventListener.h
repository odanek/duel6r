//
// Created by marcellus on 21.6.2015.
//

#ifndef DUEL6R_TEAMDEATHMATCHPLAYEREVENTLISTENER_H
#define DUEL6R_TEAMDEATHMATCHPLAYEREVENTLISTENER_H

#include "../Type.h"
#include "../PlayerEventListener.h"

namespace Duel6
{
    class PlayerEventListener;
    class InfoMessageQueue;
    class Player;
    class Shot;
    class GameSettings;

    class TeamDeathMatchPlayerEventListener : public PlayerEventListener
    {
    private:
        bool friendlyFire;
    public:
        TeamDeathMatchPlayerEventListener(InfoMessageQueue*messageQueue, GameSettings* gameSettings, bool friendlyFire)
            : PlayerEventListener(messageQueue, gameSettings), friendlyFire(friendlyFire)
        { }

        ~TeamDeathMatchPlayerEventListener() { }

        bool onDamageByShot(Duel6::Player &player, Duel6::Player &shootingPlayer, Duel6::Float32 amount, Duel6::Shot &shot, bool directHit);

        void onKillByPlayer(Player &player, Player &killer, Shot &shot, bool suicice);
    };
}


#endif //DUEL6R_TEAMDEATHMATCHPLAYEREVENTLISTENER_H
