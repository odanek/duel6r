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

#include "Person.h"

namespace Duel6
{
	Person& Person::reset()
	{
		shots = 0;
		hits = 0;
		kills = 0;
		wins = 0;
		games = 0;
		return *this;
	}

	void Person::serialize(File& file) const
	{
		Uint32 nameLength = getName().length();
		file.write(&nameLength, 4, 1);
		file.write(getName().c_str(), 1, nameLength);
		file.write(&shots, 4, 1);
		file.write(&hits, 4, 1);
		file.write(&kills, 4, 1);
		file.write(&wins, 4, 1);
		file.write(&penalties, 4, 1);
		file.write(&games, 4, 1);
	}

	void Person::deSerialize(File& file)
	{
		Uint32 nameLength;
		file.read(&nameLength, 4, 1);

		name.clear();
		name.reserve(nameLength);
		while (nameLength-- > 0)
		{
			char letter;
			file.read(&letter, 1, 1);
			name.push_back(letter);
		}

		file.read(&shots, 4, 1);
		file.read(&hits, 4, 1);
		file.read(&kills, 4, 1);
		file.read(&wins, 4, 1);
		file.read(&penalties, 4, 1);
		file.read(&games, 4, 1);
	}
}