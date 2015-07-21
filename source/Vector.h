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

#ifndef DUEL6_VECTOR_H
#define DUEL6_VECTOR_H

#include <algorithm>
#include "Math.h"
#include "Type.h"

namespace Duel6
{
	class Vector
	{
	public:
		Float32 x;
		Float32 y;

	public:
		Vector()
		{}

		explicit Vector(Float32 val)
			: x(val), y(val)
		{}

		Vector(Float32 x, Float32 y)
			: x(x), y(y)
		{}

		Vector(Int32 x, Int32 y)
			: x(Float32(x)), y(Float32(y))
		{}

		Vector(const Vector& v)
			: x(v.x), y(v.y)
		{}

		Vector& operator=(const Vector& v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}

		Float32 setY() const
		{
			return y;
		}

		Vector& set(Float32 x, Float32 y)
		{
			this->x = x;
			this->y = y;
			return *this;
		}

		Vector& setX(Float32 x)
		{
			this->x = x;
			return *this;
		}

		Vector setY(Float32 y)
		{
			this->y = y;
			return *this;
		}

		Vector operator+(const Vector& v) const
		{
			return Vector(x + v.x, y + v.y);
		}

		Vector& operator+=(const Vector& v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}

		Vector operator-(const Vector& v) const
		{
			return Vector(x - v.x, y - v.y);
		}

		Vector operator-() const
		{
			return Vector(-x, -y);
		}

		Vector& operator-=(const Vector& v)
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}

		Vector operator*(Float32 val) const
		{
			return Vector(x * val, y * val);
		}

		Vector& operator*=(Float32 val)
		{
			x *= val;
			y *= val;
			return *this;
		}

		Vector operator*(const Vector& v) const
		{
			return Vector(x * v.x, y * v.y);
		}

		Vector& operator*=(const Vector& v)
		{
			x *= v.x;
			y *= v.y;
			return *this;
		}

		Vector operator/(Float32 val) const
		{
			return Vector(x / val, y / val);
		}

		Vector& operator/=(Float32 val)
		{
			x /= val;
			y /= val;
			return *this;
		}

		Vector& operator/=(const Vector& v)
		{
			x /= v.x;
			y /= v.y;
			return *this;
		}

		Float32 dot(const Vector& v) const
		{
			return x * v.x + y * v.y;
		}

		Float32 length() const
		{
			return Math::norm(x, y);
		}

		Vector abs() const
		{
			return Vector(std::abs(x), std::abs(y));
		}

		Vector unit() const
		{
			return (*this) / length();
		}

		static Vector direction(Int32 angle)
		{
			return Vector(Math::fastSin(90 + angle), Math::fastSin(angle));
		}

		static Vector min(const Vector& left, const Vector& right)
		{
			return Vector(std::min(left.x, right.x), std::min(left.y, right.y));
		}

		static Vector max(const Vector& left, const Vector& right)
		{
			return Vector(std::max(left.x, right.x), std::max(left.y, right.y));
		}
	};

	inline Vector operator*(Float32 val, const Vector& vec)
	{
		return Vector(val * vec.x, val * vec.y);
	}
}

#endif