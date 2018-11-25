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
            Int32 causedDamage = std::min((Int32) amount, (Int32) player.getLife());
            shootingPlayer.getPerson().addDamageCaused(causedDamage);
            attackers[&player][&shootingPlayer].hits++;
            attackers[&player][&shootingPlayer].totalDamage += causedDamage;
            attackers[&player][&shootingPlayer].player = &shootingPlayer;
            ShotHit hit = shot.getShotHit();
            if (hit.collidingPlayer != nullptr && !player.is(*hit.collidingPlayer)) {
                // Player detonating the missile gets the assistance
                auto &entry = attackers[&player][hit.collidingPlayer];
                entry.hits++;
                entry.totalDamage += causedDamage;
                entry.player = hit.collidingPlayer;
            }
            if (hit.collidingShotPlayer != nullptr && !player.is(*hit.collidingShotPlayer)) {
                // Player shooting down a missile gets the assistance
                auto &entry = attackers[&player][hit.collidingShotPlayer];
                entry.hits++;
                entry.totalDamage += causedDamage;
                entry.player = hit.collidingShotPlayer;
            }
        }
        player.addLife(-amount);
        return true;
    }

    bool PlayerEventListener::onDamageByEnv(Player &player, Float32 amount) {
        player.addLife(-amount);
        return true;
    }

    void PlayerEventListener::onKillByPlayer(Player &player, Player &killer, Shot &shot, bool suicide) {
        onKill(player, killer, shot, suicide);

        auto assistants = attackers[&player];
        assistants.erase(&killer);

        auto qualifiedAssistances = getQualifiedAssistances(assistants);

        if (qualifiedAssistances.size() > 0) {
            onAssistedKill(player, killer, qualifiedAssistances, suicide);
        }
        addKillMessage(player, killer, qualifiedAssistances, suicide);
        player.getPerson().addDeaths(1);
    }

    void PlayerEventListener::onKill(Player &player, Player &killer, Shot &shot, bool suicide) {
        if (suicide) {
            if (gameSettings.getScreenMode() == ScreenMode::SplitScreen) {
                messageQueue.add(player, Format("killed by suicide of [{0}]") << killer.getPerson().getName());
            }
            killer.getPerson().addPenalties(1);
        } else {
            if (gameSettings.getScreenMode() == ScreenMode::SplitScreen) {
                messageQueue.add(player, Format("killed by [{0}]") << killer.getPerson().getName());
            }
            killer.getPerson().addKills(1);
            killer.addRoundKills(1);
        }
    }

    void PlayerEventListener::addKillMessage(Player &killed, Player &killer, const AssistanceList &assistances,
                                             bool suicide) {
        if (suicide) {
            //handled by addSuicideMessage()
        } else {
            std::string assistedByMessage = "";
            if (assistances.size() > 0) {
                assistedByMessage = ", assisted by: ";
                bool first = true;
                for (auto assistance : assistances) {
                    if (first) {
                        first = false;
                    } else {
                        assistedByMessage += ", ";
                    }
                    assistedByMessage += assistance.player->getPerson().getName();
                }
            }
            messageQueue.add(killer, Format("killed [{0}]{1}") << killed.getPerson().getName() << assistedByMessage);
        }
    }

    void PlayerEventListener::onKillByEnv(Player &player) {
        //TODO: Change of behavior - when killed by bonus, player gets a penalty point!
        //TODO: Fix by providing enviroment type
        player.getPerson().addPenalties(1);
        player.getPerson().addDeaths(1);
        messageQueue.add(player, "You are dead");
    }

    void PlayerEventListener::onSuicide(Player &player, std::vector<Player *> &playersKilled) {
        player.getPerson().addPenalties(1);
        player.getPerson().addDeaths(1);

        auto qualifiedAssistances = getQualifiedAssistances(attackers[&player]);

        onAssistedSuicide(player, qualifiedAssistances);
        addSuicideMessage(player, qualifiedAssistances, playersKilled);
    }

    void PlayerEventListener::addSuicideMessage(Player &player, const AssistanceList &assistances,
                                                std::vector<Player *> &playersKilled) {
        std::string assistedMessage = "";
        std::string killedAlsoMessage = "";

        if (assistances.size() > 0) {
            bool first = true;
            for (auto assistance : assistances) {
                if (first) {
                    first = false;
                    assistedMessage = ", assisted by: ";
                } else {
                    assistedMessage += ", ";
                }
                assistedMessage += assistance.player->getPerson().getName();
            }
        }

        bool first = true;
        for (auto killed: playersKilled) {
            if (killed != &player) {
                if (first) {
                    first = false;
                    killedAlsoMessage = ", killed also: ";
                } else {
                    killedAlsoMessage += ", ";
                }
                killedAlsoMessage += killed->getPerson().getName();
            }
        }
        messageQueue.add(player, Format("Commited suicide{0}{1}") << assistedMessage << killedAlsoMessage);
    }

    void PlayerEventListener::onAssistedSuicide(Player &player, const AssistanceList &assistances) {
        for (auto assistance : assistances) {
            assistance.confirm();
        }
    }

    void PlayerEventListener::onRoundWin(Player &player) {
        messageQueue.add(player, "You won the round");
        player.getPerson().addWins(1);
    }

    void PlayerEventListener::onAssistedKill(Player &killed, Player &killer, const AssistanceList &assistances,
                                             bool suicide) {
        for (auto assistance : assistances) {
            assistance.confirm();
        }
    }

    PlayerEventListener::AssistanceList PlayerEventListener::getQualifiedAssistances(const AssistantsMap &assistants) {
        AssistanceList qualifiedAssistances;
        for (auto assistant : assistants) {
            if (assistant.second.totalDamage > D6_MAX_LIFE * 0.4) {
                qualifiedAssistances.push_back(assistant.second);
            }
        }
        return qualifiedAssistances;
    }
}
