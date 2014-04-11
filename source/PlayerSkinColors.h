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

#ifndef DUEL6_PLAYERSKINCOLORS_H
#define DUEL6_PLAYERSKINCOLORS_H

#include "Color.h"

namespace Duel6
{
	class PlayerSkinColors
	{
	public:
		enum BodyPart
		{
			HairTop = 0,
			HairBottom,
			BodyOuter,
			BodyInner,
			HandOuter,
			HandInner,
			Trousers,
			Shoes,
			Face
		};

	private:
		Color m_color[9];

	public:
		PlayerSkinColors()
		{}

		PlayerSkinColors(const Color& color)
		{
			for (Size i = 0; i < 9; i++)
			{
				m_color[i] = color;
			}
		}

		PlayerSkinColors(const PlayerSkinColors& skin)
		{
			*this = skin;
		}

		PlayerSkinColors& operator=(const PlayerSkinColors& skin)
		{
			if (this != &skin)
			{
				for (Size i = 0; i < 9; i++)
				{
					m_color[i] = skin.m_color[i];
				}
			}

			return *this;
		}

		PlayerSkinColors& Set(BodyPart bodyPart, const Color& color)
		{
			m_color[(int)bodyPart] = color;
			return *this;
		}

		const Color& Get(BodyPart bodyPart) const
		{
			return m_color[(int)bodyPart];
		}
	};
}

#endif