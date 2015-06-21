//
// Created by marcellus on 21.6.2015.
//

#include "PredatorPlayerEventListener.h"
#include "Predator.h"

bool Duel6::PredatorPlayerEventListener::onDamageByShot(Duel6::Player &player, Duel6::Player &shootingPlayer, Duel6::Float32 amount, Duel6::Shot &shot, bool directHit)
{
    if (player.hasTeam(Duel6::Predator::PREDATOR_TEAM()))
    {
        amount *= 0.3;
    }

    return PlayerEventListener::onDamageByShot(player, shootingPlayer, amount, shot, directHit);
}
