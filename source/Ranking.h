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

#ifndef DUEL6_RANKING_H
#define DUEL6_RANKING_H

#include <string>
#include <vector>
#include "Color.h"
#include "Type.h"

namespace Duel6 {
    class Ranking {
    public:
        class Entry {
        public:
            std::string name;
            Int32 points;
            Int32 wins = 0;
            Int32 kills = 0;
            Int32 deaths = 0;
            Int32 assistances = 0;
            Int32 penalties = 0;
            Color fontColor;
            Color bcgColor;
            std::vector<Entry> entries;

        public:
            Entry(const std::string &name, Int32 points, const Color &fontColor, const Color &bcgColor)
                    : name(name), points(points), fontColor(fontColor), bcgColor(bcgColor), entries() {}

            void addSubEntry(const Entry &subEntry) {
                entries.push_back(subEntry);
            }

            bool isSuperEntry() const {
                return entries.size() > 0;
            }
        };

    public:
        std::vector<Entry> entries;

    public:
        Ranking() = default;

        Int32 getMaxLength() {
            Int32 maxLength = 0;
            for (auto &entry : entries) {
                maxLength = std::max(maxLength, Int32(entry.name.size()));
                for (auto &subEntry : entry.entries) {
                    maxLength = std::max(maxLength, Int32(subEntry.name.size()));
                }
            }
            return maxLength;
        }
    };
}

#endif
