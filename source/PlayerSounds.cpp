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

#include <stdlib.h>
#include <unordered_map>
#include "json/JsonParser.h"
#include "Defines.h"
#include "EnumClassHash.h"
#include "PlayerSounds.h"
#include "math/Math.h"

namespace Duel6 {
    namespace {
        std::unordered_map<PlayerSounds::Type, std::vector<std::string>, EnumClassHash<PlayerSounds::Type>> defaultSounds = {
                {PlayerSounds::Type::GotHit,      {"hit.wav"}},
                {PlayerSounds::Type::WasKilled,   {"death.wav"}},
                {PlayerSounds::Type::Suicide,     {"death.wav"}},
                {PlayerSounds::Type::Drowned,     {"death.wav"}},
                {PlayerSounds::Type::PickedBonus, {"pick-bonus.wav"}},
                {PlayerSounds::Type::DoubleJump,  {"djump.ogg", "djump02.ogg",
                    "djump03.ogg",
                    "djump04.ogg",
                    "djump05.ogg",
                    "djump06.ogg",
                    "djump07.ogg",
                    "djump08.ogg",
                    "djump09.ogg",
                    "djump10.ogg",
                    "djump11.ogg",
                    "djump12.ogg",
                    "djump13.ogg"
                }}
        };

        Sound::Sample emptySample;
        std::unordered_map<PlayerSounds::Type, std::vector<Sound::Sample>, EnumClassHash<PlayerSounds::Type>> defaultSamples;

        std::vector<Sound::Sample> loadDefaultSounds(Sound &sound, PlayerSounds::Type type) {
            std::vector<Sound::Sample> result;
            auto soundFiles = defaultSounds.find(type);
            if (soundFiles == defaultSounds.end()) {
                result.push_back(emptySample);
                return result;
            }

            auto samples = defaultSamples.find(type);
            if (samples == defaultSamples.end()) {
                for(const auto & soundFile: soundFiles->second){
                    Sound::Sample defaultSample = sound.loadSample(D6_FILE_PLAYER_SOUNDS + soundFile);
                    result.push_back(defaultSample);
                }
                defaultSamples.insert(std::make_pair(type, result));
                return result;
            }

            return samples->second;
        }

        Sound::Sample
        loadSound(Sound &sound, const std::string &profileRoot, PlayerSounds::Type type, Json::Value value) {
            return sound.loadSample(profileRoot + value.asString());
        }

        std::vector<Sound::Sample>
        loadSounds(Sound &sound, const std::string &profileRoot, PlayerSounds::Type type, Json::Value value) {
            std::vector<Sound::Sample> samples;

            if (value.getType() == Json::Value::Type::Array) {
                for (Size i = 0; i < value.getLength(); i++) {
                    samples.push_back(loadSound(sound, profileRoot, type, value.get(i)));
                }
            } else {
                if (value.getType() == Json::Value::Type::Null) {
                    auto defaultSamples = loadDefaultSounds(sound, type);
                    samples.insert(samples.end(), defaultSamples.begin(), defaultSamples.end());
                } else {
                    samples.push_back(loadSound(sound, profileRoot, type, value));
                }
            }

            return samples;
        }
    }

    PlayerSounds::PlayerSounds() {
    }

    const Sound::Sample &PlayerSounds::getRandomSample(Type type) const {
        const std::vector<Sound::Sample> &samples = sounds[(Int32) type];
        Size which = (Size) Math::random(Int32(samples.size()));
        return samples[which];
    }

    PlayerSounds PlayerSounds::load(Sound &sound, const std::string &profilePath, const std::string &file) {
        Json::Parser parser;
        Json::Value root = parser.parse(profilePath + file);

        PlayerSounds sounds;
        sounds.sounds[(Int32) Type::GotHit] = loadSounds(sound, profilePath, Type::GotHit, root.get("gotHit"));
        sounds.sounds[(Int32) Type::WasKilled] = loadSounds(sound, profilePath, Type::WasKilled, root.get("wasKilled"));
        sounds.sounds[(Int32) Type::HitOther] = loadSounds(sound, profilePath, Type::HitOther, root.get("hitOther"));
        sounds.sounds[(Int32) Type::KilledOther] = loadSounds(sound, profilePath, Type::KilledOther, root.get("killedOther"));
        sounds.sounds[(Int32) Type::Suicide] = loadSounds(sound, profilePath, Type::Suicide, root.get("suicide"));
        sounds.sounds[(Int32) Type::Drowned] = loadSounds(sound, profilePath, Type::Drowned, root.get("drowned"));
        sounds.sounds[(Int32) Type::PickedBonus] = loadSounds(sound, profilePath, Type::PickedBonus, root.get("pickedBonus"));
        sounds.sounds[(Int32) Type::DoubleJump] = loadSounds(sound, profilePath, Type::DoubleJump, root.getOrDefault("doubleJump", Json::Value::makeNull()));
        return sounds;
    }

    PlayerSounds PlayerSounds::makeDefault(Sound &sound) {
        PlayerSounds sounds;
        sounds.sounds[(Int32) Type::GotHit] = loadDefaultSounds(sound, Type::GotHit);
        sounds.sounds[(Int32) Type::WasKilled] = loadDefaultSounds(sound, Type::WasKilled);
        sounds.sounds[(Int32) Type::HitOther]= loadDefaultSounds(sound, Type::HitOther);
        sounds.sounds[(Int32) Type::KilledOther]= loadDefaultSounds(sound, Type::KilledOther);
        sounds.sounds[(Int32) Type::Suicide]= loadDefaultSounds(sound, Type::Suicide);
        sounds.sounds[(Int32) Type::Drowned]= loadDefaultSounds(sound, Type::Drowned);
        sounds.sounds[(Int32) Type::PickedBonus]= loadDefaultSounds(sound, Type::PickedBonus);
        sounds.sounds[(Int32) Type::DoubleJump]= loadDefaultSounds(sound, Type::DoubleJump);
        return sounds;
    }
}