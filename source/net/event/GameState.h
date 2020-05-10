/*
 * RequestGameState.h
 *
 *  Created on: Mar 19, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_GAMESTATE_H_
#define SOURCE_NET_EVENT_GAMESTATE_H_

#include <list>
#include "../Event.h"
#include "../object/Client.h"
#include "../object/World.h"
#include "../object/Player.h"
#include "../object/Bonus.h"
#include "../object/Weapon.h"
#include "PlayerProfile.h"
namespace Duel6::net {
    enum class GameStateState : type_t {
        WAITING_FOR_PLAYERS,
        RUNNING,
        MAX_COUNT
    };

    template<typename Stream>
    bool serialize(Stream &s, GameStateState &o) {
        if (s.isDeserializer()) {
            type_t r;
            if (!s.safe_max(r, (type_t) (static_cast<type_t>(GameStateState::MAX_COUNT) - 1))) {
                return false;
            }
            o = static_cast<GameStateState>(r);
            return true;
        } else {
            type_t r = static_cast<type_t>(o);
            return s & r;
        }
    }
    class GameState: public Event<GameState, EventType::GAME_STATE> {
    public:
        GameStateState state = GameStateState::RUNNING;
    //    std::vector<Client> clients;
        Int32 clientId;
        tick_t tick;
        World world;
        std::vector<PlayerProfile> playerProfiles;
        std::vector<Player> players;
        std::vector<Bonus> bonuses;
        std::vector<Weapon> weapons;
        GameState() = default;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & state // @suppress("Suggested parenthesis around expression")
                && s & clientId // @suppress("Suggested parenthesis around expression")
                && s & tick // @suppress("Suggested parenthesis around expression")
                && s & world // @suppress("Suggested parenthesis around expression")
                && s & playerProfiles // @suppress("Suggested parenthesis around expression")
                && s & players // @suppress("Suggested parenthesis around expression")
                && s & bonuses // @suppress("Suggested parenthesis around expression")
                && s & weapons; // @suppress("Suggested parenthesis around expression")
        }
    };
}

#endif
