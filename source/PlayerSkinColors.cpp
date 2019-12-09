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

#include "json/JsonParser.h"
#include "PlayerSkinColors.h"
#include "math/Math.h"

namespace Duel6 {
    namespace {
        static Color hairColors[] = {
                Color(255, 255, 0),
                Color(106, 72, 0),
                Color(51, 0, 0),
                Color(240, 0, 0),
                Color(0, 187, 0),
                Color(106, 72, 80)
        };

        static Color faceColors[] = {
                Color(255, 145, 172),
                Color(255, 216, 116),
                Color(153, 0, 0)
        };

        static Color headBandColors[] = {
                Color(224, 36, 36),
                Color(222, 146, 40),
                Color(40, 144, 122)
        };
    }

    PlayerSkinColors::PlayerSkinColors(const Color &color) {
        for (Size i = 0; i < 11; i++) {
            this->color[i] = color;
        }
    }
    PlayerSkinColors::PlayerSkinColors(const PlayerSkinColors &colors){
        for (Size i = 0; i < 11; i++) {
            this->color[i] = colors.color[i];
        }
        this->hair = colors.hair;
        this->headBand = colors.headBand;
    }

    PlayerSkinColors &PlayerSkinColors::operator=(const PlayerSkinColors &colors) {
        for (Size i = 0; i < 11; i++) {
            this->color[i] = colors.color[i];
        }
        this->hair = colors.hair;
        this->headBand = colors.headBand;
        return *this;
    }


    PlayerSkinColors PlayerSkinColors::load(const std::string &profileRoot, const std::string &file) {
        Json::Parser parser;
        Json::Value root = parser.parse(profileRoot + file);

        PlayerSkinColors colors;
        colors.set(PlayerSkinColors::HairTop, Color::fromString(root.get("hairTop").asString()));
        colors.set(PlayerSkinColors::HairBottom, Color::fromString(root.get("hairBottom").asString()));
        colors.set(PlayerSkinColors::BodyInner, Color::fromString(root.get("bodyInner").asString()));
        colors.set(PlayerSkinColors::BodyOuter, Color::fromString(root.get("bodyOuter").asString()));
        colors.set(PlayerSkinColors::HandInner, Color::fromString(root.get("handInner").asString()));
        colors.set(PlayerSkinColors::HandOuter, Color::fromString(root.get("handOuter").asString()));
        colors.set(PlayerSkinColors::Trousers, Color::fromString(root.get("trousers").asString()));
        colors.set(PlayerSkinColors::Shoes, Color::fromString(root.get("shoes").asString()));
        colors.set(PlayerSkinColors::Face, Color::fromString(root.get("face").asString()));
        colors.set(PlayerSkinColors::HeadBand, Color::fromString(root.getOrDefault("headBandColor", Json::Value::makeString("e02424")).asString()));
        colors.setHair(root.getOrDefault("hair", Json::Value::makeNumber(0)).asInt());
        colors.setHeadBand(root.getOrDefault("headBand", Json::Value::makeBoolean(false)).asBoolean());
        return colors;
    }

    PlayerSkinColors PlayerSkinColors::makeRandom() {
        const Color& hair = hairColors[Math::random(6)];
        const Color& face = faceColors[Math::random(3)];
        const Color& headBand = headBandColors[Math::random(3)];
        Color body = Color::random();

        PlayerSkinColors colors;
        colors.set(PlayerSkinColors::HairTop, hair);
        colors.set(PlayerSkinColors::HairBottom, hair.scale(0.5));
        colors.set(PlayerSkinColors::BodyInner, body);
        colors.set(PlayerSkinColors::BodyOuter, body.scale(0.5));
        colors.set(PlayerSkinColors::HandInner, body);
        colors.set(PlayerSkinColors::HandOuter, body.scale(0.2));
        colors.set(PlayerSkinColors::Trousers, Color::random());
        colors.set(PlayerSkinColors::Shoes, Color::random());
        colors.set(PlayerSkinColors::Face, face);
        colors.set(PlayerSkinColors::HeadBand, headBand);

        colors.setHair(Math::random(3));
        colors.setHeadBand(Math::random(10) > 5);
        return colors;
    }
}
