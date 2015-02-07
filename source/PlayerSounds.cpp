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

#include <unordered_map>
#include "json/Json.h"
#include "Defines.h"
#include "EnumClassHash.h"
#include "PlayerSounds.h"

namespace Duel6
{
	namespace
	{
		std::unordered_map<PlayerSounds::Type, std::string, EnumClassHash<PlayerSounds::Type>> defaultSounds = {
			{ PlayerSounds::Type::GotHit, "hit.wav" },
			{ PlayerSounds::Type::WasKilled, "death.wav" },
			{ PlayerSounds::Type::Suicide, "death.wav" },
			{ PlayerSounds::Type::Drowned, "death.wav" },
			{ PlayerSounds::Type::PickedBonus, "pick-bonus.wav" }
		};

		Sound::Sample emptySample;
		std::unordered_map<PlayerSounds::Type, Sound::Sample, EnumClassHash<PlayerSounds::Type>> defaultSamples;

		Sound::Sample loadSound(Sound& sound, const std::string& profileRoot, PlayerSounds::Type type, const Json::Value& value)
		{
			if (value.getType() == Json::Value::Type::Null)
			{
				auto soundFile = defaultSounds.find(type);
				if (soundFile == defaultSounds.end())
				{
					return emptySample;
				}

				auto sample = defaultSamples.find(type);
				if (sample == defaultSamples.end())
				{
					Sound::Sample defaultSample = sound.loadSample(D6_FILE_PLAYER_SOUNDS + soundFile->second);
					defaultSamples.insert(std::make_pair(type, defaultSample));
					return defaultSample;
				}

				return sample->second;
			}

			return sound.loadSample(profileRoot + value.asString());
		}
	}

	PlayerSounds PlayerSounds::load(Sound& sound, const std::string& profilePath, const std::string& file)
	{
		Json::Parser parser;
		const Json::Value& root = parser.parse(profilePath + file);

		PlayerSounds sounds;
		sounds.sounds[(Int32)Type::GotHit] = loadSound(sound, profilePath, Type::GotHit, root.get("gotHit"));
		sounds.sounds[(Int32)Type::WasKilled] = loadSound(sound, profilePath, Type::WasKilled, root.get("wasKilled"));
		sounds.sounds[(Int32)Type::HitOther] = loadSound(sound, profilePath, Type::HitOther, root.get("hitOther"));
		sounds.sounds[(Int32)Type::KilledOther] = loadSound(sound, profilePath, Type::KilledOther, root.get("killedOther"));
		sounds.sounds[(Int32)Type::Suicide] = loadSound(sound, profilePath, Type::Suicide, root.get("suicide"));
		sounds.sounds[(Int32)Type::Drowned] = loadSound(sound, profilePath, Type::Drowned, root.get("drowned"));
		sounds.sounds[(Int32)Type::PickedBonus] = loadSound(sound, profilePath, Type::PickedBonus, root.get("pickedBonus"));
		return sounds;
	}
}