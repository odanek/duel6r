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

#include "TeamDeathMatchPlayerEventListener.h"

namespace Duel6 {
    bool TeamDeathMatchPlayerEventListener::onDamageByShot(Player &player, Player &shootingPlayer, Float32 amount,
                                                           Shot &shot, bool directHit) {

        if(player.getTeam() == 0 || shootingPlayer.getTeam() == 0) {
            return PlayerEventListener::onDamageByShot(player, shootingPlayer, amount, shot, directHit);
        }
        if (!friendlyFire && &player != &shootingPlayer) {

            const Team *playerTeam = teamMap.at(&player);
            const Team *shooterTeam = teamMap.at(&shootingPlayer);
            if (playerTeam == shooterTeam) {
                return false;
            }
        }

        return PlayerEventListener::onDamageByShot(player, shootingPlayer, amount, shot, directHit);
    }

    void TeamDeathMatchPlayerEventListener::onKill(Player &player, Player &killer, Shot &shot, bool suicide) {
        if(player.getTeam() == 0 || killer.getTeam() == 0) {
            return PlayerEventListener::onKill(player, killer, shot, suicide);
        }
        const Team *playerTeam = teamMap.at(&player);
        const Team *killerTeam = teamMap.at(&killer);

        if (!suicide && playerTeam == killerTeam) {
            killer.getPerson().addPenalties(1);
        } else {
            PlayerEventListener::onKill(player, killer, shot, suicide);
        }
    }

    void TeamDeathMatchPlayerEventListener::onAssistedSuicide(Player &player, const AssistanceList &assistances) {
        if(player.getTeam() == 0){
            return;
        }
        const Team *playerTeam = teamMap.at(&player);
        for (auto assistance : assistances) {
            if(assistance.player->getTeam() == 0){
                continue;
            }
            if(teamMap.at(assistance.player) != playerTeam){
                assistance.confirm();
            }
        }
    }

    void TeamDeathMatchPlayerEventListener::onAssistedKill(Player &killed, Player &killer, const AssistanceList &assistances, bool suicide) {
        if(killed.getTeam() == 0 || killer.getTeam() == 0){
            return PlayerEventListener::onAssistedKill(killed, killer, assistances, suicide);
        }

        const Team *playerTeam = teamMap.at(&killed);
        const Team *killerTeam = teamMap.at(&killer);

        if (playerTeam != killerTeam) {
            //No assistance points for kamikazed kills
            if(suicide){
                return;
            }
            for (auto assistance : assistances) {
                if(assistance.player->getTeam() == 0){
                    continue;
                }
                const Team *assistantTeam = teamMap.at(assistance.player);
                if (killerTeam == assistantTeam || globalAssistances) {
                    assistance.confirm();
                }
            }
        } else {
            for (auto assistance : assistances) {
                if(assistance.player->getTeam() == 0){
                    continue;
                }
                const Team *assistantTeam = teamMap.at(assistance.player);
                if (killerTeam != assistantTeam) {
                    assistance.confirm();
                }
            }
        }
    }

    void TeamDeathMatchPlayerEventListener::addKillMessage(Player &killed, Player &killer, const AssistanceList &assistances, bool suicide) {
        if (suicide || killed.getTeam() == 0 || killer.getTeam() == 0) {
            return PlayerEventListener::addKillMessage(killed, killer, assistances, suicide);
        }
        const Team *playerTeam = teamMap.at(&killed);
        const Team *killerTeam = teamMap.at(&killer);

        std::string assistedByMessage = "";
        std::string killedMessage = "killed";

        AssistanceList filteredAssistances;

        std::function<bool (Assistance)> predicate =([&](Assistance assistance) {return (globalAssistances && playerTeam != teamMap.at(assistance.player)) || killerTeam == teamMap.at(assistance.player);});

        if (playerTeam == killerTeam) {
            killedMessage = "killed teammate";
            predicate = ([&](Assistance assistance) {return killerTeam != teamMap.at(assistance.player);});;
        }
        //filter assistants by predicate
        std::copy_if(assistances.begin(), assistances.end(), std::inserter(filteredAssistances, filteredAssistances.end()), predicate);

        if (filteredAssistances.size() > 0) {
            bool first = true;
            for (auto assistance : filteredAssistances) {
                if (first) {
                    assistedByMessage = ", assisted by: ";
                    first = false;
                } else {
                    assistedByMessage += ", ";
                }
                assistedByMessage += assistance.player->getPerson().getName();
            }
        }
        messageQueue.add(killer, Format("{0} [{1}]{2}") << killedMessage << killed.getPerson().getName() << assistedByMessage);
    }
}
