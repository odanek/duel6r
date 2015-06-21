//
// Created by marcellus on 21.6.2015.
//

#ifndef DUEL6R_INVISIBLEMANPLAYEREVENTLISTENER_H
#define DUEL6R_INVISIBLEMANPLAYEREVENTLISTENER_H

#include "../Type.h"
#include "../PlayerEventListener.h"

namespace Duel6
{
    class PlayerEventListener;
    class InfoMessageQueue;
    class Player;
    class Shot;
    class GameSettings;

    class PredatorPlayerEventListener : public PlayerEventListener
    {
    public:
        PredatorPlayerEventListener(InfoMessageQueue*messageQueue, GameSettings* gameSettings)
                : PlayerEventListener(messageQueue, gameSettings)
        { }

        bool onDamageByShot(Duel6::Player &player, Duel6::Player &shootingPlayer, Duel6::Float32 amount, Duel6::Shot &shot, bool directHit);
    };
}


#endif //DUEL6R_INVISIBLEMANPLAYEREVENTLISTENER_H
