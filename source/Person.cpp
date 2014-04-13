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
	Person& Person::Reset()
	{
		m_shots = 0;
		m_hits = 0;
		m_kills = 0;
		m_wins = 0;
		m_games = 0;
		return *this;
	}

	void Person::Serialize(FILE* file) const
	{
		Uint32 nameLength = Name().length();
		fwrite(&nameLength, 4, 1, file);
		fwrite(Name().c_str(), 1, nameLength, file);
		fwrite(&m_shots, 4, 1, file);
		fwrite(&m_hits, 4, 1, file);
		fwrite(&m_kills, 4, 1, file);
		fwrite(&m_wins, 4, 1, file);
		fwrite(&m_games, 4, 1, file);
	}

	void Person::DeSerialize(FILE* file)
	{
		Uint32 nameLength;
		fread(&nameLength, 4, 1, file);

		m_name.clear();
		m_name.reserve(nameLength);
		while (nameLength-- > 0)
		{
			char letter;
			fread(&letter, 1, 1, file);
			m_name.push_back(letter);
		}

		fread(&m_shots, 4, 1, file);
		fread(&m_hits, 4, 1, file);
		fread(&m_kills, 4, 1, file);
		fread(&m_wins, 4, 1, file);
		fread(&m_games, 4, 1, file);
	}
}