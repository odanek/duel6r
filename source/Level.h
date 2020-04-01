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

#ifndef DUEL6_LEVEL_H
#define DUEL6_LEVEL_H

#include <string>
#include <queue>
#include <vector>
#include "Block.h"
#include "Water.h"
#include "Elevator.h"
#include "Type.h"
namespace Duel6 {
    namespace net{
        class Level;
        class ClientGameProxy;
        class ServerGameProxy;
    }
    class Game;

    class Level {
        friend class Duel6::net::Level;
        friend class Duel6::net::ClientGameProxy;
        friend class Duel6::net::ServerGameProxy;
    public:
        typedef std::pair<Int32, Int32> StartingPosition;
        typedef std::vector<StartingPosition> StartingPositionList;

    private:
        Block::Meta *blockMeta;
        Int32 width;
        Int32 height;
        std::string background;
        Uint16 waterBlock;
        std::vector<Uint16> levelData;
        Int32 waterLevel;
        bool raisingWater;
        std::vector<Elevator> elevators;

    public:
        void setBlockMeta(Block::Meta & ref){
            blockMeta = &ref;
        }
        Level(Level &&);
        Level(const Int32 width, const Int32 height,
              const std::string & background,
              const Uint16 waterBlock,
              const std::vector<Uint16> & levelData,
              Int32 waterLevel,
              bool raisingWater,
              const std::vector<Elevator> & elevators);

        Level(const std::string &path, bool mirror, Block::Meta &blockMeta);
~Level() {
    //waat
}
        Int32 getWidth() const {
            return width;
        }

        Int32 getHeight() const {
            return height;
        }

        const std::vector<Uint16> &getLevelData() const {
            return levelData;
        }

        Uint16 getWaterBlock() const {
            return waterBlock;
        }

        std::string getBackground() const {
            return background;
        }

        std::vector<Elevator> &getElevators();

        bool isEmpty(Int32 x, Int32 y) const {
            return isInside(x, y) ? getBlockMeta(x, y).is(Block::Type::EmptySpace) : false;
        }

        bool isWater(Int32 x, Int32 y) const {
            return isInside(x, y) ? getBlockMeta(x, y).is(Block::Type::Water) : false;
        }

        bool isWall(Int32 x, Int32 y, bool outside) const {
            return isInside(x, y) ? getBlockMeta(x, y).is(Block::Type::Wall) : outside;
        }

        bool isWall(Float32 x, Float32 y, bool outside) const {
            Int32 ix = (Int32) floorf(x);
            Int32 iy = (Int32) floorf(y);
            return isWall(ix, iy, outside);
        }

        bool isInside(Int32 x, Int32 y) const {
            return (x >= 0 && x < width && y >= 0 && y < height);
        }

        const Block &getBlockMeta(Int32 x, Int32 y) const {
            return (*blockMeta)[getBlock(x, y)];
        }

        const Water *getWaterType(Int32 x, Int32 y) const;

        void raiseWater();

        void findStartingPositions(StartingPositionList &startingPositions);

        void findTopmostNonWallPositions(StartingPositionList &startingPositions);

        Int32 getWaterLevel() const;

        bool isRaisingWater() const;

    private:
        void load(const std::string &path, bool mirror);

        void mirrorLevelData();

        bool isPossibleStartingPosition(Int32 x, Int32 y);

        Uint16 getBlock(Int32 x, Int32 y) const {
            return levelData[(height - y - 1) * width + x];
        }

        void setBlock(Uint16 block, Int32 x, Int32 y) {
            levelData[(height - y - 1) * width + x] = block;
        }

        Uint16 findWaterType() const;

        Int32 findWaterLevel(Uint16 waterBlock) const;
    };
}

#endif
