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

#ifndef DUEL6_FIRE_H
#define DUEL6_FIRE_H

#include <vector>
#include <unordered_map>
#include "Type.h"
#include "FaceList.h"
#include "TextureManager.h"
#include "SpriteList.h"
#include "GameResources.h"

namespace Duel6 {
    class FireType {
    public:
        static const FireType CONIFEROUS_TREE;
        static const FireType BROAD_LEAVED_TREE;

    private:
        static const std::vector<FireType> types;
        Size id;
        Size block;

    private:
        FireType(Size id, Size block)
                : id(id), block(block) {}

    public:
        static const std::vector<FireType> &values() {
            return types;
        }

        Size getId() const {
            return id;
        }

        Size getBlock() const {
            return block;
        }
    };

    class Fire {
    private:
        const FireType &type;
        Face &face;
        Vector position;
        bool burned;

    public:
        Fire(const FireType &type, Face &face, const Vector &position);

        const FireType &getType() const {
            return type;
        }

        const Vector &getPosition() const {
            return position;
        }

        Face &getFace() const {
            return face;
        }

        bool isBurned() const {
            return burned;
        }

        void setBurned(bool burned) {
            this->burned = burned;
        }
    };

    class FireList {
    private:
        SpriteList &spriteList;
        const std::unordered_map<Size, TextureList> &textures;
        std::vector<Fire> fires;

    public:
        FireList(const GameResources &resources, SpriteList &spriteList);

        void find(FaceList &sprites);

        void check(const Vector &explCentre, Float32 d);
    };
}

#endif
