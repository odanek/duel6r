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

#include "Sprite.h"

namespace Duel6
{
	Sprite::Sprite()
	{
		frame = 0;
		delay = 0;
		speed = 1;
		looping = AnimationLooping::RepeatForever;
		orientation = Orientation::Left;
		visible = true;
		noDepth = false;
		finished = false;
		size = Vector(1.0f, 1.0f);
		grow = 0;
		alpha = 1.0f;
	}

	Sprite::Sprite(const Int16* animation, const TextureManager::Texture& textures)
		: Sprite()
	{
		this->animation = animation;
		this->textures = &textures.getGlTextures()[0];
	}

	Sprite& Sprite::setAnimation(const Int16* animation)
	{
		if (this->animation != animation)
		{
			this->animation = animation;
			delay = 0.0f;
			frame = 0;
			finished = false;
		}

		return *this;
	}

	Sprite& Sprite::setDraw(bool draw)
	{
		visible = draw;
		return *this;
	}

	Sprite& Sprite::setNoDepth(bool depth)
	{
		noDepth = depth;
		return *this;
	}

	void Sprite::update(float elapsedTime)
	{
		delay += elapsedTime;
		if (delay >= animation[frame + 1] * speed)
		{
			frame += 2;
			delay = 0;
			if (animation[frame] == -1)
			{
				finished = true;

				if (looping == AnimationLooping::RepeatForever)
				{
					frame = 0;
				}
				else if (looping == AnimationLooping::OnceAndStop)
				{
					frame -= 2;
				}
			}
		}

		if (grow > 0)
		{
			Vector growStep = Vector(grow, grow) * elapsedTime;
			position -= growStep;
			size += 2 * growStep;
		}
	}

	void Sprite::render() const
	{
		if (!visible)
		{
			return;
		}

		if (isNoDepth())
		{
			glDisable(GL_DEPTH_TEST);
		}

		GLfloat cur_col[4];
		glGetFloatv(GL_CURRENT_COLOR, cur_col);

		glColor4f(cur_col[0], cur_col[1], cur_col[2], alpha);

		glBindTexture(GL_TEXTURE_2D, textures[animation[frame]]);

		float leftSide = (orientation == Orientation::Left) ? 0.0f : 1.0f;

		glBegin(GL_QUADS);
			glTexCoord2f(leftSide, 0.0f);
			glVertex3f(position.x, position.y + size.y, z);;
			glTexCoord2f(1.0f - leftSide, 0.0f);
			glVertex3f(position.x + size.x, position.y + size.y, z);;
			glTexCoord2f(1.0f - leftSide, 1.0f);
			glVertex3f(position.x + size.x, position.y, z);
			glTexCoord2f(leftSide, 1.0f);
			glVertex3f(position.x, position.y, z);
		glEnd();

		glColor4f(cur_col[0], cur_col[1], cur_col[2], 1.0f);

		if (isNoDepth())
		{
			glEnable(GL_DEPTH_TEST);
		}
	}
}