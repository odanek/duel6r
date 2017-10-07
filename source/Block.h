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

#ifndef DUEL6_BLOCK_H
#define DUEL6_BLOCK_H

#include <vector>
#include <string>
#include "Type.h"

namespace Duel6 {
    class Block {
    public:
        typedef std::vector<Block> Meta;

        enum Type {
            EmptySpace = 0,
            Wall,
            Water,
            FrontSprite,
            BackSprite,
            FrontAndBackSprite,
            Front4Sprite,
            Back4Sprite,
            Waterfall
        };

    private:
        Size index;
        Type type;
        std::vector<Int32> textures;

    public:
        Block(Size index, Type type, const std::vector<Int32> &textures)
                : index(index), type(type), textures(textures) {}

        Block(Size index, Type type, std::vector<Int32> &&textures)
                : index(index), type(type), textures(textures) {}

        Size getIndex() const {
            return index;
        }

        Type getType() const {
            return type;
        }

        const std::vector<Int32> &getTextures() const {
            return textures;
        }

        Size getAnimationFrames() const {
            return textures.size();
        }

        bool is(Type type) const {
            return (this->type == type);
        }

        static Meta loadMeta(const std::string &path);

    private:
        static Type determineType(const std::string &kind);
    };
}

#endif