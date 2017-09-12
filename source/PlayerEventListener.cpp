/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "PlayerEventListener.h"
#include "Weapon.h"

namespace Duel6 {
    bool PlayerEventListener::onDamageByShot(Player &player, Player &shootingPlayer, Float32 amount, Shot &shot,
                                             bool directHit) {
        if (!player.is(shootingPlayer)) {
            shootingPlayer.getPerson().addDamageCaused(std::min((Int32) amount, (Int32) player.getLife()));
        }
        player.addLife(-amount);
        return true;
    }

    bool PlayerEventListener::onDamageByEnv(Player &player, Float32 amount) {
        player.addLife(-amount);
        return true;
    }

    void PlayerEventListener::onKillByPlayer(Player &player, Player &killer, Shot &shot, bool suicide) {
        if (suicide) {
            messageQueue.add(killer, Format("killed also [{0}]") << player.getPerson().getName());
            if (gameSettings.getScreenMode() == ScreenMode::SplitScreen) {
                messageQueue.add(player, Format("killed by suicide of [{0}]") << killer.getPerson().getName());
            }
            killer.getPerson().addPenalties(1);
        } else {
            messageQueue.add(killer, Format("killed [{0}]") << player.getPerson().getName());
            if (gameSettings.getScreenMode() == ScreenMode::SplitScreen) {
                messageQueue.add(player, Format("killed by [{0}]") << killer.getPerson().getName());
            }
            killer.getPerson().addKills(1);
            killer.addRoundKills(1);
        }
    }


    void PlayerEventListener::onKillByEnv(Player &player) {
        //TODO: Change of behavior - when killed by bonus, player gets a penalty point!
        //TODO: Fix by providing enviroment type
        player.getPerson().addPenalties(1);
        messageQueue.add(player, "You are dead");
    }

    void PlayerEventListener::onSuicide(Player &player, Size otherKilledPlayers) {
        player.getPerson().addPenalties(1);
        messageQueue.add(player, "Committed suicide");
    }

    void PlayerEventListener::onRoundWin(Player &player) {
        messageQueue.add(player, "You won the round");
        player.getPerson().addWins(1);
    }
}
