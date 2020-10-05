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

#include "Lightning.h"
#include "LightningShot.h"

namespace Duel6 {
    namespace {
        const LegacyWeapon::Definition DEFINITION = {100, 4.1f,
                                                     "lightning",
                                                     "blesk.wav", "bmblesk.wav",
                                                     {1, 82, 2, 82, 3, 82, 4, 82, 0, 82, 0, 82, 0, 820, -1, 0}};
    }

    Lightning::Lightning(Sound &sound, TextureManager &textureManager)
            : LegacyWeapon(sound, textureManager, DEFINITION, 2) {}

    Float32 Lightning::getBulletSpeed() const {
        return 12.2f;
    }
    std::unique_ptr<Shot> Lightning::makeShot(Player &player, World &world, Orientation orientation,
                                   const Weapon &weapon,
                                   Uint32 shotId,
                                   bool powerful,
                                   Int32 power, Float32 bulletSpeed,
                                   Vector &position,
                                   Vector &velocity) const {
        return std::make_unique<LightningShot>(player, world, *this, orientation,
            weapon, shotId, powerful, power, bulletSpeed, position, velocity);
    }
    std::unique_ptr<Shot> Lightning::makeShot(Player &player, World &world, Orientation orientation) const {
        return std::make_unique<LightningShot>(player, world, *this, orientation);
    }
}
