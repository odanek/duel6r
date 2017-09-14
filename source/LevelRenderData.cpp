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


#include <algorithm>
#include "LevelRenderData.h"

namespace Duel6 {
    LevelRenderData::LevelRenderData(const Level &level, Float32 animationSpeed, Float32 waveHeight)
            : level(level), animationSpeed(animationSpeed), animWait(0), waveHeight(waveHeight) {}

    void LevelRenderData::generateFaces() {
        addWallFaces();
        addSpriteFaces();
        generateWater();
    }

    void LevelRenderData::generateWater() {
        addWaterFaces();
        floatingVertexes.build(water, waveHeight);
    }

    void LevelRenderData::update(Float32 elapsedTime) {
        animWait += elapsedTime;
        if (animWait > animationSpeed) {
            animWait = 0;
            walls.nextFrame();
            sprites.nextFrame();
            water.nextFrame();
        }

        floatingVertexes.update(elapsedTime);
    }

    void LevelRenderData::addWallFaces() {
        walls.clear();

        for (Int32 y = 0; y < level.getHeight(); y++) {
            for (Int32 x = 0; x < level.getWidth(); x++) {
                const Block &block = level.getBlockMeta(x, y);

                if (block.is(Block::Wall)) {
                    addWall(block, x, y);
                }
            }
        }

        walls.optimize();
    }

    void LevelRenderData::addSpriteFaces() {
        sprites.clear();

        for (Int32 y = 0; y < level.getHeight(); y++) {
            for (Int32 x = 0; x < level.getWidth(); x++) {
                const Block &block = level.getBlockMeta(x, y);

                if (block.is(Block::FrontAndBackSprite)) {
                    addSprite(sprites, block, x, y, 1.0f);
                    addSprite(sprites, block, x, y, 0.0f);
                } else if (block.is(Block::FrontSprite)) {
                    addSprite(sprites, block, x, y, 1.0f);
                } else if (block.is(Block::BackSprite)) {
                    addSprite(sprites, block, x, y, 0.0f);
                } else if (block.is(Block::Front4Sprite)) {
                    addSprite(sprites, block, x, y, 0.75f);
                } else if (block.is(Block::Back4Sprite)) {
                    addSprite(sprites, block, x, y, 0.25f);
                }
            }
        }

        sprites.optimize();
    }

    void LevelRenderData::addWaterFaces() {
        water.clear();

        for (Int32 y = 0; y < level.getHeight(); y++) {
            for (Int32 x = 0; x < level.getWidth(); x++) {
                const Block &block = level.getBlockMeta(x, y);

                if (block.is(Block::Waterfall)) {
                    addSprite(water, block, x, y, 0.75);
                } else if (block.is(Block::Water)) {
                    addWater(block, x, y);
                }
            }
        }

        water.optimize();
    }

    void LevelRenderData::addWall(const Block &block, Int32 x, Int32 y) {
        walls.addFace(Face(block))
                .addVertex(Vertex(0, x, y + 1, 1))
                .addVertex(Vertex(1, x + 1, y + 1, 1))
                .addVertex(Vertex(2, x + 1, y, 1))
                .addVertex(Vertex(3, x, y, 1));


#ifdef D6_RENDER_BACKS
        faceList.addFace(Face(block))
            .addVertex(Vertex(0, x + 1, y + 1, 0))
            .addVertex(Vertex(1, x, y + 1, 0))
            .addVertex(Vertex(2, x, y, 0))
            .addVertex(Vertex(3, x + 1, y, 0));
#endif

        if (!level.isWall(x - 1, y, false)) {
            walls.addFace(Face(block))
                    .addVertex(Vertex(0, x, y + 1, 0))
                    .addVertex(Vertex(1, x, y + 1, 1))
                    .addVertex(Vertex(2, x, y, 1))
                    .addVertex(Vertex(3, x, y, 0));
        }
        if (!level.isWall(x + 1, y, false)) {
            walls.addFace(Face(block))
                    .addVertex(Vertex(0, x + 1, y + 1, 1))
                    .addVertex(Vertex(1, x + 1, y + 1, 0))
                    .addVertex(Vertex(2, x + 1, y, 0))
                    .addVertex(Vertex(3, x + 1, y, 1));
        }
        if (!level.isWall(x, y + 1, false)) {
            walls.addFace(Face(block))
                    .addVertex(Vertex(0, x, y + 1, 1))
                    .addVertex(Vertex(1, x, y + 1, 0))
                    .addVertex(Vertex(2, x + 1, y + 1, 0))
                    .addVertex(Vertex(3, x + 1, y + 1, 1));
        }
        if (!level.isWall(x, y - 1, false)) {
            walls.addFace(Face(block))
                    .addVertex(Vertex(0, x, y, 1))
                    .addVertex(Vertex(1, x + 1, y, 1))
                    .addVertex(Vertex(2, x + 1, y, 0))
                    .addVertex(Vertex(3, x, y, 0));
        }
    }

    void LevelRenderData::addWater(const Block &block, Int32 x, Int32 y) {
        bool topWater = !level.isWater(x, y + 1);
        Vertex::Flag flowFlag = topWater ? Vertex::Flag::Flow : Vertex::Flag::None;

        water.addFace(Face(block))
                .addVertex(Vertex(0, x, y + 1, 1, flowFlag))
                .addVertex(Vertex(1, x + 1, y + 1, 1, flowFlag))
                .addVertex(Vertex(2, x + 1, y, 1))
                .addVertex(Vertex(3, x, y, 1));

        water.addFace(Face(block))
                .addVertex(Vertex(0, x + 1, y + 1, 0, flowFlag))
                .addVertex(Vertex(1, x, y + 1, 0, flowFlag))
                .addVertex(Vertex(2, x, y, 0))
                .addVertex(Vertex(3, x + 1, y, 0));

        if (topWater) {
            water.addFace(Face(block))
                    .addVertex(Vertex(0, x, y + 1, 1, Vertex::Flag::Flow))
                    .addVertex(Vertex(1, x, y + 1, 0, Vertex::Flag::Flow))
                    .addVertex(Vertex(2, x + 1, y + 1, 0, Vertex::Flag::Flow))
                    .addVertex(Vertex(3, x + 1, y + 1, 1, Vertex::Flag::Flow));
        }
    }

    void LevelRenderData::addSprite(FaceList &faceList, const Block &block, Int32 x, Int32 y, Float32 z) {
        Float32 fx = Float32(x), fy = Float32(y);
        bool bottomWaterfall = (block.is(Block::Waterfall) && level.isWater(x, y - 1));
        Vertex::Flag flowFlag = bottomWaterfall ? Vertex::Flag::Flow : Vertex::Flag::None;

        faceList.addFace(Face(block))
                .addVertex(Vertex(0, fx, fy + 1, z))
                .addVertex(Vertex(1, fx + 1, fy + 1, z))
                .addVertex(Vertex(2, fx + 1, fy, z, flowFlag))
                .addVertex(Vertex(3, fx, fy, z, flowFlag));
    }
}