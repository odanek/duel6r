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

#include "SpriteList.h"

namespace Duel6
{
	SpriteIterator SpriteList::AddSprite(const Sprite& sprite)
	{
		m_sprites.push_back(sprite);
		return std::prev(m_sprites.end());
	}

	void SpriteList::Update(Float32 elapsedTime)
	{
		// Update everything
		for (Sprite& sprite : m_sprites)
		{
			sprite.Update(elapsedTime);
		}

		// Delete sprites with finished animations
		auto sprite = m_sprites.begin();
		while (sprite != m_sprites.end())
		{
			if (sprite->Looping() == AnimationLooping::OnceAndRemove && sprite->IsFinished())
			{
				sprite = m_sprites.erase(sprite);
			}
			else
			{
				++sprite;
			}
		}
	}

	void SpriteList::Render() const
	{
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_CULL_FACE);

		RenderTransparent(false);

		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);

		RenderTransparent(true);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	void SpriteList::RenderTransparent(bool transparent) const
	{
		for (const Sprite& sprite : m_sprites)
		{
			if (sprite.IsTransparent() == transparent)
			{
				sprite.Render();
			}
		}
	}
}