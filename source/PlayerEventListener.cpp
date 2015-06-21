//
// Created by marcellus on 20.6.2015.
//

#include "PlayerEventListener.h"
#include "Player.h"
#include "InfoMessageQueue.h"
#include "Weapon.h"

bool Duel6::PlayerEventListener::onDamageByShot(Duel6::Player &player, Duel6::Player &shootingPlayer,
                                                Duel6::Float32 amount,
                                                Duel6::Shot &shot, bool directHit)
{
    player.addLife(-amount);
    return true;
}

bool Duel6::PlayerEventListener::onDamageByEnv(Duel6::Player &player, Duel6::Float32 amount)
{
    player.addLife(-amount);
    return true;
}

void Duel6::PlayerEventListener::onKillByPlayer(Player& player, Player& killer, Shot &shot, bool suicide)
{

    if (suicide)
    {
        this->messageQueue->add(killer, Format(D6_L("killed also [{0}]")) << player.getPerson().getName());
        killer.getPerson().addPenalties(1);
    }
    else
    {
        this->messageQueue->add(killer, Format(D6_L("killed [{0}]")) << player.getPerson().getName());
        killer.getPerson().addKills(1);
        killer.addRoundKills(1);
    }
}


void Duel6::PlayerEventListener::onKillByEnv(Duel6::Player &player)
{
    //TODO: Change of behavior - when killed by bonus, player gets a penalty point!
    //TODO: Fix by providing enviroment type
    player.getPerson().addPenalties(1);
    messageQueue->add(player, D6_L("You are dead"));
}

void Duel6::PlayerEventListener::onSuicide(Duel6::Player &player, int otherKilledPlayers)
{
    player.getPerson().addPenalties(1);
    messageQueue->add(player, D6_L("Committed suicide"));
}

void Duel6::PlayerEventListener::onRoundWin(Duel6::Player &player)
{
    messageQueue->add(player, D6_L("You won the round"));
    player.getPerson().addWins(1);
}
