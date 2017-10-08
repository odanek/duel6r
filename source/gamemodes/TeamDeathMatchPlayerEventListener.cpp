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
        const Team *playerTeam = teamMap.at(&player);
        const Team *killerTeam = teamMap.at(&killer);

        if (!suicide && playerTeam == killerTeam) {
            killer.getPerson().addPenalties(1);
        } else {
            PlayerEventListener::onKill(player, killer, shot, suicide);
        }
    }
    void TeamDeathMatchPlayerEventListener::onAssistedSuicide(Player &player, const std::set<Player *> &assistants) {
        const Team *playerTeam = teamMap.at(&player);
        for (auto assistant : assistants) {
            if(teamMap.at(assistant) != playerTeam){
                assistant->getPerson().addAssistances(1);
            }
        }
    }
    void TeamDeathMatchPlayerEventListener::onAssistedKill(Player &killed, Player &killer, const std::set<Player *> &assistants, bool suicide) {
        const Team *playerTeam = teamMap.at(&killed);
        const Team *killerTeam = teamMap.at(&killer);

        if (playerTeam != killerTeam) {
            //No assistance points for kamikazed kills
            if(suicide){
                return;
            }
            for (auto assistant : assistants) {
                const Team *assistantTeam = teamMap.at(assistant);
                if (killerTeam == assistantTeam) {
                    assistant->getPerson().addAssistances(1);
                }
            }
        } else {
            for (auto assistant : assistants) {
                const Team *assistantTeam = teamMap.at(assistant);
                if (killerTeam != assistantTeam) {
                    assistant->getPerson().addAssistances(1);
                }
            }
        }
    }

    void TeamDeathMatchPlayerEventListener::addKillMessage(Player &killed, Player &killer, const std::set<Player *> &assistants, bool suicide) {
        if (suicide) {
            return PlayerEventListener::addKillMessage(killed, killer, assistants, suicide);
        }
        const Team *playerTeam = teamMap.at(&killed);
        const Team *killerTeam = teamMap.at(&killer);

        std::string assistedByMessage = "";
        std::string killedMessage = "killed";

        std::set<Player *> filteredAssistants;

        std::function<bool (Player *)> predicate =([&](Player * value) {return killerTeam == teamMap.at(value);});

        if (playerTeam == killerTeam) {
            killedMessage = "killed teammate";
            predicate = ([&](Player * value) {return killerTeam != teamMap.at(value);});;
        }
        //filter assistants by predicate
        std::copy_if(assistants.begin(), assistants.end(), std::inserter(filteredAssistants, filteredAssistants.end()), predicate);

        if (filteredAssistants.size() > 0) {
            bool first = true;
            for (auto assistant : filteredAssistants) {
                if (first) {
                    assistedByMessage = ", assisted by: ";
                    first = false;
                } else {
                    assistedByMessage += ", ";
                }
                assistedByMessage += assistant->getPerson().getName();
            }
        }
        messageQueue.add(killer, Format("{0} [{1}]{2}") << killedMessage << killed.getPerson().getName() << assistedByMessage);
    }
}
