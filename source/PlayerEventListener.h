//
// Created by marcellus on 20.6.2015.
//

#ifndef DUEL6R_PLAYEREVENTLISTENER_H
#define DUEL6R_PLAYEREVENTLISTENER_H


#include "Type.h"

namespace Duel6
{

    class Player;
    class InfoMessageQueue;
    class Shot;

    class PlayerEventListener
    {
    protected:
        InfoMessageQueue* messageQueue;

    public:

        PlayerEventListener(InfoMessageQueue*messageQueue)
                : messageQueue(messageQueue)
        { }

        /**
         * Event which is being triggered when player takes damage from another player.
         * Return false if hit does not count.
         */
        virtual bool onDamageByShot(Player &player, Player &shootingPlayer, Float32 amount, Shot &shot, bool directHit);

        /**
         * Event which is being triggered when player takes damage from environment.
         * Return false if hit does not count.
         */
        virtual bool onDamageByEnv(Player &player, Float32 amount);

        virtual void onKillByPlayer(Player &player, Player &killer, Shot &shot, bool suicide);

        //TODO: Environment type (eg. water, lava, bonus)
        virtual void onKillByEnv(Player &player);

        virtual void onSuicide(Duel6::Player &player, int otherKilledPlayers);

        virtual void onRoundWin(Player& player);
    };
}

#endif //DUEL6R_PLAYEREVENTLISTENER_H
