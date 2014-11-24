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

#include <vector>
#include <SDL2/SDL_opengl.h>
#include "Globals.h"
#include "Math.h"
#include "FaceList.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "SpriteList.h"

#define D6_FIRES        2

namespace Duel6
{
	struct FireType
	{
		Int32 index;
		Int32 block;
		std::string textureKey;
	};

	struct Fire
	{
		Float32 x;
		Float32 y;
		Face* face;
		FireType* type;
		bool burned;
	};

	FireType d6FireTypes[D6_FIRES] = 
	{
		{ 0, 11 },
		{ 1, 12 }
	};
	std::vector<Fire> d6Fires;
	Int16 d6FireAnm[20] = { 0, 20, 1, 20, 0, 20, 1, 20, 0, 20, 1, 20, 0, 20, 1, 20, 2, 100, -1, 0 };

	void FIRE_Init()
	{
		for (FireType& ft : d6FireTypes)
		{
			ft.textureKey = "fire_" + std::to_string(ft.index);
			d6TextureManager.load(ft.textureKey, Format("{0}{1,3|0}/") << D6_TEXTURE_FIRE_PATH << ft.index, GL_LINEAR, true);

			glBindTexture(GL_TEXTURE_2D, d6TextureManager.get(ft.textureKey)[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

	void FIRE_Find(FaceList& sprites)
	{
		d6Fires.clear();

		Size i = 0;
		for (Face& face : sprites.getFaces())
		{
			for (FireType& ft : d6FireTypes)
			{
				if (face.getBaseTexture() == ft.block)
				{
					Fire fire;
					fire.x = sprites.getVertexes()[i << 2].x;
					fire.y = sprites.getVertexes()[i << 2].y - 1.0f;
					fire.burned = false;
					fire.face = &face;
					fire.type = &ft;
					d6Fires.push_back(fire);
				}
			}

			i++;
		}
	}

	void FIRE_Check(Float32 X, Float32 Y, Float32 d)  // TODO: Coord - explosionCenter
	{
		X -= 0.5f;
		Y += 0.5f;

		const TextureManager::TextureList& textures = d6TextureManager.get(D6_TEXTURE_BLOCK_KEY);
		for (Fire& fire : d6Fires)
		{
			if (!fire.burned)
			{
				Float32 distance = Math::distance(X, Y, fire.x, fire.y);

				if (distance < d)
				{
					fire.burned = true;
					fire.face->hide();

					Sprite fireSprite(d6FireAnm, d6TextureManager.get(fire.type->textureKey));
					fireSprite.setPosition(fire.x, fire.y, 0.75f)
						.setLooping(AnimationLooping::OnceAndStop);
					d6SpriteList.addSprite(fireSprite);
				}
			}
		}
	}
}