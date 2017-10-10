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

#ifndef DUEL6_PLAYEREVENTLISTENER_H
#define DUEL6_PLAYEREVENTLISTENER_H

#include <map>
#include <list>

#include "InfoMessageQueue.h"
#include "GameSettings.h"
#include "Player.h"
#include "Shot.h"

namespace Duel6 {
    struct Assistance {
        Player * player;
        Int32 totalDamage;
        Int32 hits;

        void confirm() {
            player->getPerson().addAssistances(1);
            player->getPerson().addAssistedDamage(totalDamage);
        }
    };

    class PlayerEventListener {
    public:
        using AssistanceList = std::list<Assistance>;
        using AssistantsMap = std::map<Player *, Assistance>;
        using AttackersMap = std::map<const Player *, AssistantsMap>;

    private:
        AssistanceList getQualifiedAssistances(const AssistantsMap &assistants);

    protected:
        InfoMessageQueue &messageQueue;
        const GameSettings &gameSettings;

        AttackersMap attackers;

        virtual void addKillMessage(Player &killed, Player &killer, const AssistanceList &assistances, bool suicide);

        virtual void addSuicideMessage(Player &player, const AssistanceList &assistances, std::vector<Player *> &playersKilled);

        virtual void onAssistedSuicide(Player &player, const AssistanceList &assistances);

        virtual void onAssistedKill(Player &killed, Player &killer, const AssistanceList &assistances, bool suicide);

        virtual void onKill(Player &player, Player &killer, Shot &shot, bool suicide);

    public:
        PlayerEventListener(InfoMessageQueue &messageQueue, const GameSettings &gameSettings)
                : messageQueue(messageQueue), gameSettings(gameSettings), attackers() {}

        virtual ~PlayerEventListener() {}

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

        virtual void onKillByPlayer(Player &player, Player &killer, Shot &shot, bool suicide) final;

        //TODO: Environment type (eg. water, lava, bonus)
        virtual void onKillByEnv(Player &player);

        virtual void onSuicide(Player &player, std::vector<Player *> &playersKilled);

        virtual void onRoundWin(Player &player);
    };
}

#endif
