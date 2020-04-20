#ifndef SOURCE_NET_EVENT_PLAYERS_DISCONNECTED_H_
#define SOURCE_NET_EVENT_PLAYERS_DISCONNECTED_H_

#include <vector>
#include "../Event.h"

namespace Duel6::net {
    class PlayersDisconnected: public Event<PlayersDisconnected, EventType::PLAYERS_DISCONNECTED>{
    public:
        std::vector<int32_t> disconnectedId;
        template<class Stream>
        bool serialize(Stream &s) {
            return s & disconnectedId;
        }
    };
}



#endif
