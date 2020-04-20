#ifndef SOURCE_NET_EVENT_PLAYERS_JOINED_H_
#define SOURCE_NET_EVENT_PLAYERS_JOINED_H_

#include <vector>
#include "../Event.h"

namespace Duel6::net {
    class PlayersJoined: public Event<PlayersJoined, EventType::PLAYERS_JOINED>{
    public:
        std::vector<PlayerProfile> playerProfiles;
        template<class Stream>
        bool serialize(Stream &s) {
            return s & playerProfiles;
        }
    };
}



#endif
