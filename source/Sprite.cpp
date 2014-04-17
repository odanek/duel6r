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
	Sprite::Sprite(const Int16* animation, const GLuint* textures)
	{
		m_animation = animation;
		m_textures = textures;
		m_frame = 0;
		m_delay = 0;
		m_speed = 1;
		m_loop = AnimationLooping::RepeatForever;
		m_orientation = Orientation::Left;
		m_flags = Draw;
		m_width = 1.0f;
		m_height = 1.0f;
		m_grow = 0;
		m_alpha = 1.0f;
	}

	Sprite& Sprite::SetAnimation(const Int16* animation)
	{
		if (m_animation != animation)
		{
			m_animation = animation;
			m_delay = 0.0f;
			m_frame = 0;
			ClearFlags(Finished);
		}

		return *this;
	}

	Sprite& Sprite::SetDraw(bool draw)
	{
		if (draw)
		{
			AddFlags(Draw);
		}
		else
		{
			ClearFlags(Draw);
		}
		return *this;
	}

	Sprite& Sprite::SetNoDepth(bool depth)
	{
		if (depth)
		{
			AddFlags(NoDepth);
		}
		else
		{
			ClearFlags(NoDepth);
		}
		return *this;
	}

	void Sprite::Update(float elapsedTime)
	{
		m_delay += elapsedTime;
		if (m_delay >= m_animation[m_frame + 1] * m_speed)
		{
			m_frame += 2;
			m_delay = 0;
			if (m_animation[m_frame] == -1)
			{
				AddFlags(Finished);

				if (m_loop == AnimationLooping::RepeatForever)
				{
					m_frame = 0;
				}
				else if (m_loop == AnimationLooping::OnceAndStop)
				{
					m_frame -= 2;
				}
			}
		}

		if (m_grow > 0)
		{
			m_x -= m_grow * elapsedTime;
			m_y += m_grow * elapsedTime;
			m_width += 2 * m_grow * elapsedTime;
			m_height += 2 * m_grow * elapsedTime;
		}
	}

	void Sprite::Render() const
	{
		if (!HasFlags(Draw))
		{
			return;
		}

		if (IsNoDepth())
		{
			glDisable(GL_DEPTH_TEST);
		}

		GLfloat cur_col[4];
		glGetFloatv(GL_CURRENT_COLOR, cur_col);
		glColor4f(cur_col[0], cur_col[1], cur_col[2], m_alpha);
		glBindTexture(GL_TEXTURE_2D, m_textures[m_animation[m_frame]]);

		float leftSide = (m_orientation == Orientation::Left) ? 0.0f : 1.0f;

		glBegin(GL_QUADS);
			glTexCoord2f(leftSide, 0.0f);
			glVertex3f(m_x, m_y, m_z);;
			glTexCoord2f(1.0f - leftSide, 0.0f);
			glVertex3f(m_x + m_width, m_y, m_z);;
			glTexCoord2f(1.0f - leftSide, 1.0f);
			glVertex3f(m_x + m_width, m_y - m_height, m_z);
			glTexCoord2f(leftSide, 1.0f);
			glVertex3f(m_x, m_y - m_height, m_z);
		glEnd();

		glColor4f(cur_col[0], cur_col[1], cur_col[2], 1.0f);

		if (IsNoDepth())
		{
			glEnable(GL_DEPTH_TEST);
		}
	}

	void Sprite::AddFlags(Uint32 flags)
	{
		m_flags |= flags;
	}

	void Sprite::ClearFlags(Uint32 flags)
	{
		m_flags &= ~flags;
	}

	bool Sprite::HasFlags(Uint32 flags) const
	{
		return (m_flags & flags) == flags;
	}
}