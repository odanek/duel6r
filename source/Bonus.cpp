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
#include "Bonus.h"
#include "Weapon.h"
#include "World.h"

namespace Duel6
{
	Bonus::Bonus(Int32 x, Int32 y, Size type, const TextureManager& textureManager)
		: x(x), y(y), type(type), weapon(false), weaponType(nullptr), bullets(0)
	{
		texture = textureManager.get(D6_TEXTURE_BONUS_KEY)[type];
	}

	Bonus::Bonus(Int32 x, Int32 y, const Weapon& weaponType, Size bullets, const TextureManager& textureManager)
		: x(x), y(y), type(0), weapon(true), weaponType(&weaponType), bullets(bullets)
	{
		texture = textureManager.get(weaponType.texture.gun)[weaponType.animation[12]];
	}

	void Bonus::render() const
	{
		Float32 rx = Float32(x);
		Float32 ry = Float32(y);

		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
			glTexCoord2f(0.1f, 0.1f); glVertex3f(rx, ry + 1, 0.5f);
			glTexCoord2f(0.9f, 0.1f); glVertex3f(rx + 1.0f, ry + 1, 0.5f);
			glTexCoord2f(0.9f, 0.9f); glVertex3f(rx + 1.0f, ry, 0.5f);
			glTexCoord2f(0.1f, 0.9f); glVertex3f(rx, ry, 0.5f);
		glEnd();
	}

}