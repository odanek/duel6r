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
#include <array>
#include "Block.h"
#include "DataException.h"
#include "json/JsonParser.h"

namespace Duel6 {
    Block::Type Block::determineType(const std::string &kind) {
        static std::string typeNames[9] = {
                "EMPTY_SPACE",
                "WALL",
                "WATER",
                "FRONT_SPRITE",
                "BACK_SPRITE",
                "FRONT_AND_BACK_SPRITE",
                "FRONT4_SPRITE",
                "BACK4_SPRITE",
                "WATERFALL"
        };

        auto typeBegin = std::begin(typeNames);
        auto typeEnd = std::end(typeNames);
        auto typeIter = std::find(typeBegin, typeEnd, kind);
        if (typeIter == typeEnd) {
            D6_THROW(DataException, std::string("Unknown block type: ") + kind);
        }

        return (Block::Type) (typeIter - typeBegin);
    }

    Block::Meta Block::loadMeta(const std::string &path) {
        Block::Meta meta;
        Json::Parser parser;
        Json::Value root = parser.parse(path);

        for (Size i = 0; i < root.getLength(); i++) {
            Json::Value block = root.get(i);
            Block::Type type = determineType(block.get("kind").asString());
            Json::Value animations = block.get("animations");
            std::vector<Int32> textures;
            for (Size j = 0; j < animations.getLength(); j++) {
                textures.push_back(animations.get(j).asInt());
            }
            meta.push_back(Block(meta.size(), type, std::move(textures)));
        }

        return meta;
    }
}