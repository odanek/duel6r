/*
 * PlaySample.h
 *
 *  Created on: May 17, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_EVENT_PLAYSAMPLE_H_
#define SOURCE_NET_EVENT_PLAYSAMPLE_H_

#include "../Event.h"
#include "../Type.h"
namespace Duel6::net {

    enum class SampleType : type_t {
        GOT_HIT,
        WAS_KILLED,
        HIT_OTHER,
        KILLED_OTHER,
        SUICIDE,
        DROWNED,
        PICKED_BONUS,
        MAX_COUNT
    };

    template<typename Stream>
    bool serialize(Stream &s, SampleType &o) {
        if (s.isDeserializer()) {
            type_t r;
            if (!s.safe_max(r, (type_t) (static_cast<type_t>(SampleType::MAX_COUNT) - 1))) {
                return false;
            }
            o = static_cast<SampleType>(r);
            return true;
        } else {
            type_t r = static_cast<type_t>(o);
            return s & r;
        }
    }

    class PlaySample: public Event<PlaySample, EventType::PLAY_SAMPLE> {
    public:
        Int32 playerId;
        SampleType sample;

        template<typename Stream>
        bool serialize(Stream &s){
            return s & playerId
                && s & sample;
        }
    };
}

#endif /* SOURCE_NET_EVENT_PLAYSAMPLE_H_ */
