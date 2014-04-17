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

#include <SDL/SDL_opengl.h>
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
		const Int16 *m_animation;    // Source array of animations and delays
		const GLuint *m_textures;   // Texture array
		Size m_frame;    // Current animation frame
		Float32 m_delay;    // Delay to next animation frame
		Float32 m_speed;     // Speed of animation
		AnimationLooping m_loop;   // Type of looping
		Orientation m_orientation;   // Current orientation
		Uint32 m_flags;  // Flags
		Float32 m_x;      // Sprite location
		Float32 m_y;
		Float32 m_z;
		Float32 m_width; // Sprite dimensions
		Float32 m_height;
		Float32 m_grow;   // Grow factor for explosions
		Float32 m_alpha;  // Transparency ratio

	public:
		Sprite(const Int16* animation, const GLuint* textures);

		Sprite& SetPosition(Float32 x, Float32 y, Float32 z)
		{
			m_x = x;
			m_y = y;
			m_z = z;
			return *this;
		}

		Sprite& SetPosition(Float32 x, Float32 y)
		{
			m_x = x;
			m_y = y;
			return *this;
		}

		Sprite& SetAnimation(const Int16* animation);

		Sprite& SetTextures(const GLuint* textures)
		{
			m_textures = textures;
			return *this;
		}

		Sprite& SetFrame(Size frame)
		{
			m_frame = frame;
			return *this;
		}

		Sprite& SetOrientation(Orientation orientation)
		{
			m_orientation = orientation;
			return *this;
		}

		Sprite& SetGrow(Float32 grow)
		{
			m_grow = grow;
			return *this;
		}

		Sprite& SetAlpha(Float32 alpha)
		{
			m_alpha = alpha;
			return *this;
		}

		Sprite& SetSpeed(Float32 speed)
		{
			m_speed = speed;
			return *this;
		}

		Sprite& SetLooping(AnimationLooping loop)
		{
			m_loop = loop;
			return *this;
		}

		Sprite& SetDraw(bool draw);

		Sprite& SetNoDepth(bool depth);

		Size Frame() const
		{
			return m_frame;
		}

		AnimationLooping Looping() const
		{
			return m_loop;
		}

		bool IsTransparent() const
		{
			return m_alpha < 1.0f;
		}

		bool IsFinished() const
		{
			return HasFlags(Finished);
		}

		bool IsNoDepth() const
		{
			return HasFlags(NoDepth);
		}

		void Update(Float32 elapsedTime);
		void Render() const;

	private:
		void AddFlags(Uint32 flags);
		void ClearFlags(Uint32 flags);
		bool HasFlags(Uint32 flags) const;
	};
}

#endif