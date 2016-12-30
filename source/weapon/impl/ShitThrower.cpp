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

#include "ShitThrowerShot.h"
#include "ShitThrower.h"

namespace Duel6
{
	namespace
	{
		LegacyWeapon::Definition DEFINITION = { 5.49f, false, false, true, Color(0, 0, 0), 2, 0, 1.97f, "shit thrower", "shit.wav", "shit-hit.wav", 0.04f, { 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, -1, 0 }, { 0, 10, 1, 10, 2, 10, 1, 10, -1, 0 }, { 0, 10, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
	}

	ShitThrower::ShitThrower(Sound& sound, TextureManager& textureManager)
		: LegacyWeapon(sound, textureManager, DEFINITION, 16)
	{
		Color brownColor(83, 44, 0);
		PlayerSkinColors skinColors(brownColor);
		brownSkin = std::make_unique<PlayerSkin>("textures/man/", skinColors, textureManager);
	}

	std::unique_ptr<Shot> ShitThrower::makeShot(Player& player, Orientation orientation, SpriteList::Iterator spriteIterator) const
	{
		return std::make_unique<ShitThrowerShot>(player, *this, orientation, spriteIterator, *brownSkin);
	}
}