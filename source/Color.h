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

#ifndef DUEL6_COLOR_H
#define DUEL6_COLOR_H

#include "Type.h"

namespace Duel6
{
	class Color
	{
	private:
		Uint8 m_color[3];

	public:
		Color()
			: Color(0, 0, 0)
		{}

		explicit Color(Uint8 value)
			: Color(value, value, value)
		{}

		Color(Uint8 red, Uint8 green, Uint8 blue)
		{
			Set(red, green, blue);
		}

		Color(const Color& color)
		{
			*this = color;
		}

		Color& operator=(const Color& color)
		{
			if (this != &color)
			{
				SetRed(color.Red());
				SetGreen(color.Green());
				SetBlue(color.Blue());
			}

			return *this;
		}

		Uint8 Red() const
		{
			return m_color[0];
		}

		Uint8 Green() const
		{
			return m_color[1];
		}

		Uint8 Blue() const
		{
			return m_color[2];
		}

		Uint8 Get(Size index) const
		{
			return m_color[index];
		}

		Color& Set(Uint8 red, Uint8 green, Uint8 blue)
		{
			SetRed(red);
			SetGreen(green);
			SetBlue(blue);
			return *this;
		}

		Color& SetRed(Uint8 value)
		{
			m_color[0] = value;
			return *this;
		}

		Color& SetGreen(Uint8 value)
		{
			m_color[1] = value;
			return *this;
		}

		Color& SetBlue(Uint8 value)
		{
			m_color[2] = value;
			return *this;
		}

		Color& Set(Size index, Uint8 value)
		{
			m_color[index] = value;
			return *this;
		}
	};
}

#endif