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

#ifndef DUEL6_PLAYERSKINCOLORS_H
#define DUEL6_PLAYERSKINCOLORS_H

#include "Color.h"

namespace Duel6 {
    class PlayerSkinColors {
    public:
        enum BodyPart {
            HairTop = 0,
            HairBottom,
            BodyOuter,
            BodyInner,
            HandOuter,
            HandInner,
            Trousers,
            Shoes,
            Face,
            HeadBand,
            HeadBandOuter
        };

        enum Hair {
            Normal = 0,
            None,
            Short
        };

    private:
        Color color[11];
        Hair hair = Normal;
        bool headBand = false;
    public:
        PlayerSkinColors() = default;

        explicit PlayerSkinColors(const Color &color);

        PlayerSkinColors(const PlayerSkinColors &colors);

        PlayerSkinColors &operator=(const PlayerSkinColors &colors);

        PlayerSkinColors &set(BodyPart bodyPart, const Color &color) {
            this->color[(int) bodyPart] = color;
            if(bodyPart == HeadBand) {
                this->color[(int) HeadBandOuter] = color.scale(0.90f);
            }
            return *this;
        }

        const Color &get(BodyPart bodyPart) const {
            return color[(int) bodyPart];
        }

        PlayerSkinColors &setHair(int value) {
            if(value < 0 || value > 2) {
                return *this;
            }
            hair = Hair(value);
            return *this;
        }

        const Hair getHair() const {
            return hair;
        }

        PlayerSkinColors &setHeadBand(bool value) {
            headBand = value;
            return *this;
        }

        const bool hasHeadBand() const {
            return headBand;
        }
        static PlayerSkinColors load(const std::string &profileRoot, const std::string &file);
        static PlayerSkinColors makeRandom();
    };
}

#endif
