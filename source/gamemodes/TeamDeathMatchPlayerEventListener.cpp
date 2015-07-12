//
// Created by marcellus on 21.6.2015.
//

#include "TeamDeathMatchPlayerEventListener.h"
#include "../Player.h"
#include "../InfoMessageQueue.h"

bool Duel6::TeamDeathMatchPlayerEventListener::onDamageByShot(Duel6::Player &player, Duel6::Player &shootingPlayer, Duel6::Float32 amount, Duel6::Shot &shot, bool directHit)
{

    if (!friendlyFire && player.hasTeam(shootingPlayer.getTeam()))
    {
        return false;
    }

    return PlayerEventListener::onDamageByShot(player, shootingPlayer, amount, shot, directHit);
}

void Duel6::TeamDeathMatchPlayerEventListener::onKillByPlayer(Duel6::Player &player, Duel6::Player &killer, Duel6::Shot &shot, bool suicide)
{
    if (!suicide && player.hasTeam(killer.getTeam()))
    {
        messageQueue->add(killer, Format("Killed teammate [{0}]") << player.getPerson().getName());
        killer.getPerson().addPenalties(1);
    }
    else
    {
        PlayerEventListener::onKillByPlayer(player, killer, shot, suicide);
    }
}