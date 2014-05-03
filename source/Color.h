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
		Uint8 color[3];

	public:
		Color()
			: Color(0, 0, 0)
		{}

		explicit Color(Uint8 value)
			: Color(value, value, value)
		{}

		Color(Uint8 red, Uint8 green, Uint8 blue)
		{
			set(red, green, blue);
		}

		Color(const Color& color)
		{
			*this = color;
		}

		Color& operator=(const Color& color)
		{
			if (this != &color)
			{
				setRed(color.getRed());
				setGreen(color.getGreen());
				setBlue(color.getBlue());
			}

			return *this;
		}

		Uint8 getRed() const
		{
			return color[0];
		}

		Uint8 getGreen() const
		{
			return color[1];
		}

		Uint8 getBlue() const
		{
			return color[2];
		}

		Uint8 get(Size index) const
		{
			return color[index];
		}

		Color& set(Uint8 red, Uint8 green, Uint8 blue)
		{
			setRed(red);
			setGreen(green);
			setBlue(blue);
			return *this;
		}

		Color& setRed(Uint8 value)
		{
			color[0] = value;
			return *this;
		}

		Color& setGreen(Uint8 value)
		{
			color[1] = value;
			return *this;
		}

		Color& setBlue(Uint8 value)
		{
			color[2] = value;
			return *this;
		}

		Color& set(Size index, Uint8 value)
		{
			color[index] = value;
			return *this;
		}
	};
}

#endif