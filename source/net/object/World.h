/*
 * World.h
 *
 *  Created on: Mar 29, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_WORLD_H_
#define SOURCE_NET_OBJECT_WORLD_H_
#include "../Object.h"
#include "Vector.h"
#include "../../Type.h"
namespace Duel6::net{
    class World : public Object<World, ObjectType::WORLD>{
    public:
        class GameSettings {
        public:
            Uint32 maxRounds;
            bool quickLiquid;
            bool globalAssistances;
            std::set<Uint8> enabledWeapons;

            template<class Stream>
            bool serialize(Stream &s) {
                return s & maxRounds &&
                    s & quickLiquid &&
                    s & globalAssistances &&
                    s & enabledWeapons;
            }

        };
        class Block {
            Size index;
            Uint8 type;
            std::vector<Int32> textures;

            Block() = default;

            template<class B>
            Block(B b)
                : index(b.index),
                  type(b.type),
                  textures(b.textures) {
            }

            template<class B>
            operator B(){
                return B(index, type, textures);
            }

            template<class Stream>
            bool serialize(Stream &s) {
                return s & index &&
                    s & type && //TODO safe_max
                    s & textures;
            }
        };

        class ControlPoint {
        public:
            Vector location;
            Float32 wait;

            ControlPoint() = default;

            template<class CP>
            ControlPoint(CP c)
                : location(c.getLocation),
                  wait(c.wait) {
            }

            template<class CP>
            operator CP() {
                return CP(location.x, location.y, wait);
            }

            template<class Stream>
            bool serialize(Stream &s) {
                return s & location &&
                    s & wait;
            }
        };
        class Elevator {
        public:
            std::vector<ControlPoint> controlPoints;
            bool circular;
            Size section;
            Float32 remainingWait;
            bool forward;
            Float32 distance;
            Float32 travelled;
            Vector position;
            Vector velocity;
            bool started;

            template<class Source>
            static Elevator from(const Source &s) {
                return {s.controlPoints,
                    s.circular,
                    s.section,
                    s.remainingWait,
                    s.forward,
                    s.distance,
                    s.travelled,
                    s.position,
                    s.velocity,
                    s.started};
            }
            template<class Target>
            operator Target() {
                return Target(
                    controlPoints,
                    circular,
                    section,
                    remainingWait,
                    forward,
                    distance,
                    travelled,
                    position,
                    velocity,
                    started);
            }
            template<class Stream>
            bool serialize(Stream &s) {
                return s & controlPoints &&
                    s & circular &&
                    s & section &&
                    s & remainingWait &&
                    s & forward &&
                    s & distance &&
                    s & travelled &&
                    s & position &&
                    s & velocity &&
                    s & started;
            }
        };

        class Level{
            Int32 width;
            Int32 height;
            std::string background;
            std::vector<Uint16> levelData;
            Uint16 waterBlock;
            Int32 waterLevel;
            bool raisingWater;
            std::vector<Elevator> elevators;

            template<class Stream>
            bool serialize(Stream &s) {
                return s & width &&
                    s & height &&
                    s & background &&
                    s & levelData &&
                    s & waterBlock &&
                    s & waterLevel &&
                    s & elevators;
            }
        };

        GameSettings gameSettings;
        Level level;

        template<class Stream>
        bool serialize(Stream &s) {
            return s & gameSettings &&
                s & level;
        }
    };
}



#endif /* SOURCE_NET_OBJECT_WORLD_H_ */
