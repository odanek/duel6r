/*
 * Level.h
 *
 *  Created on: Mar 30, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_OBJECT_LEVEL_H_
#define SOURCE_NET_OBJECT_LEVEL_H_

#include <vector>
#include "../Object.h"
#include "Vector.h"
#include "../../Elevator.h"
namespace Duel6::net {

    class ControlPoint {
    public:
        Vector location;
        Float32 wait;

        ControlPoint() = default;
        ControlPoint(const ControlPoint&) = default;
        ControlPoint(ControlPoint&) = default;
        ControlPoint& operator=(const ControlPoint&) = default;
        ControlPoint& operator=(ControlPoint&&) = default;

        template<class CP>
        ControlPoint(CP c)
            : location(c.location),
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


        Elevator() = default;
        Elevator(const Elevator&) = default;
        Elevator(Elevator&) = default;
        Elevator& operator =(const Elevator&) = default;
        Elevator& operator =(Elevator&&) = default;
        Elevator(std::vector<ControlPoint> & controlPoints,
                 bool circular,
                 Size section,
                 Float32 remainingWait,
                 bool forward,
                 Float32 distance,
                 Float32 travelled,
                 Vector &&position,
                 Vector &&velocity,
                 bool started)
            : controlPoints(controlPoints),
              circular(circular),
              section(section),
              remainingWait(remainingWait),
              forward(forward),
              distance(distance),
              travelled(travelled),
              position(position),
              velocity(velocity),
              started(started)
        {

        }
        template<class Source>
        Elevator(const Source &s)
            : controlPoints(s.controlPoints.begin(), s.controlPoints.end()),
              circular(s.circular),
              section(s.section),
              remainingWait(s.remainingWait),
              forward(s.forward),
              distance(s.distance),
              travelled(s.travelled),
              position(s.position),
              velocity(s.velocity),
              started(s.started) {
        }
//        operator Duel6::Elevator() {
//            return Duel6::Elevator()
//        }
//        template<class Target>
//        operator Target() {
//            return Target(
//                controlPoints,
//                circular,
//                section,
//                remainingWait,
//                forward,
//                distance,
//                travelled,
//                position,
//                velocity,
//                started);
//        }
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
    class Level: public Object<Level, ObjectType::LEVEL> {
    public:
        Int32 width;
        Int32 height;
        std::string background;
        std::vector<Uint16> levelData;
        Uint16 waterBlock;
        Int32 waterLevel;
        bool raisingWater;
        std::vector<Elevator> elevators;

        Level() = default;
        Level(const Level&) = default;
        Level(Level&&) = default;
        Level& operator =(const Level&) = default;
        Level& operator =(Level&&) = default;
        template<class Source>
        Level(Source &s)
            :
              width(s.getWidth()),
              height(s.getHeight()),
              background(s.getBackground()),
              levelData(s.getLevelData()),
              waterBlock(s.getWaterBlock()),
              waterLevel(s.getWaterLevel()),
              raisingWater(s.isRaisingWater())//,
             // elevators(s.getElevators().begin(), s.getElevators().end())
              {
        }
//        template<class Target>
//        operator Target() {
//           return Target(width,
//              height,
//              levelData,
//              waterBlock,
//              background,
//              elevators);
//        }
        template<class Stream>
        bool serialize(Stream &s) {
            return
                s & width &&
                s & height &&
                s & background &&
                s & levelData &&
                s & waterBlock &&
                s & waterLevel &&
                s & elevators;
        }
    };
}

#endif /* SOURCE_NET_OBJECT_LEVEL_H_ */
