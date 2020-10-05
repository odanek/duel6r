
#ifndef SOURCE_NET_EVENT_PLAYERINPUTSUPDATE_H_
#define SOURCE_NET_EVENT_PLAYERINPUTSUPDATE_H_

#include "../Event.h"
#include "../../Type.h"
#include "../object/PlayerInputs.h"

namespace Duel6::net {
    class PlayerInputsUpdate : public Event<PlayerInputsUpdate, EventType::PLAYER_INPUTS_UPDATE>{
    public:
        tick_t inputTick;
        tick_t confirmInputTick;
        std::vector<PlayerInputs> playersInputs;
        template<typename Stream>
        bool serialize(Stream &s) {
            return s & inputTick
                && s & confirmInputTick
                && s & playersInputs;
        }
    };
}



#endif /* SOURCE_NET_EVENT_GAMESTATEUPDATE_H_ */
