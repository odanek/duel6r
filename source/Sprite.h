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

#ifndef DUEL6_SPRITE_H
#define DUEL6_SPRITE_H

#include <vector>
#include <SDL2/SDL_opengl.h>
#include "Type.h"
#include "AnimationLooping.h"
#include "Orientation.h"

namespace Duel6
{
	class Sprite
	{
	private:
		enum Flags
		{
			Draw = 0x01,         // Don't render this sprite
			NoDepth = 0x02,      // Disable depth test
			Finished = 0x04      // The animation has finished
		};

	private:
		const Int16* animation;    // Source array of animations and delays
		const GLuint* textures;   // Texture array
		Size frame;    // Current animation frame
		Float32 delay;    // Delay to next animation frame
		Float32 speed;     // Speed of animation
		AnimationLooping looping;   // Type of looping
		Orientation orientation;   // Current orientation
		Uint32 flags;  // Flags
		Float32 x;      // Sprite location
		Float32 y;
		Float32 z;
		Float32 width; // Sprite dimensions
		Float32 height;
		Float32 grow;   // Grow factor for explosions
		Float32 alpha;  // Transparency ratio

	public:
		Sprite(const Int16* animation, const std::vector<GLuint>& textures);

		Sprite& setPosition(Float32 x, Float32 y, Float32 z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			return *this;
		}

		Sprite& setPosition(Float32 x, Float32 y)
		{
			this->x = x;
			this->y = y;
			return *this;
		}

		Sprite& setAnimation(const Int16* animation);

		Sprite& setTextures(const std::vector<GLuint>& textures)
		{
			this->textures = &textures[0];
			return *this;
		}

		Sprite& setFrame(Size frame)
		{
			this->frame = frame;
			return *this;
		}

		Sprite& setOrientation(Orientation orientation)
		{
			this->orientation = orientation;
			return *this;
		}

		Sprite& setGrow(Float32 grow)
		{
			this->grow = grow;
			return *this;
		}

		Sprite& setAlpha(Float32 alpha)
		{
			this->alpha = alpha;
			return *this;
		}

		Sprite& setSpeed(Float32 speed)
		{
			this->speed = speed;
			return *this;
		}

		Sprite& setLooping(AnimationLooping looping)
		{
			this->looping = looping;
			return *this;
		}

		Sprite& setDraw(bool draw);

		Sprite& setNoDepth(bool depth);

		Size getFrame() const
		{
			return frame;
		}

		AnimationLooping getLooping() const
		{
			return looping;
		}

		bool isTransparent() const
		{
			return alpha < 1.0f;
		}

		bool isFinished() const
		{
			return hasFlags(Finished);
		}

		bool isNoDepth() const
		{
			return hasFlags(NoDepth);
		}

		void update(Float32 elapsedTime);
		void render() const;

	private:
		void addFlags(Uint32 flags);
		void clearFlags(Uint32 flags);
		bool hasFlags(Uint32 flags) const;
	};
}

#endif