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

#ifndef DUEL6_PERSON_H
#define DUEL6_PERSON_H

#include <stdio.h>
#include <string>
#include "Type.h"
#include "File.h"

namespace Duel6
{
	class Person
	{
	private:
		std::string name;
		Int32 shots;
		Int32 hits;
		Int32 kills;
		Int32 wins;
		Int32 games;

	public:
		Person()
			: shots(0), hits(0), kills(0), wins(0), games(0)
		{}

		explicit Person(const std::string& name)
			: Person()
		{
			this->name = name;
		}

		const std::string& getName() const
		{
			return name;
		}

		Int32 getShots() const
		{
			return shots;
		}

		Int32 getHits() const
		{
			return hits;
		}

		Int32 getKills() const
		{
			return kills;
		}

		Int32 getWins() const
		{
			return wins;
		}

		Int32 getGames() const
		{
			return games;
		}

		Int32 getTotalPoints() const
		{
			return getKills() + getWins();
		}

		Person& setShots(Int32 shots)
		{
			this->shots = shots;
			return *this;
		}

		Person& setHits(Int32 hits)
		{
			this->hits = hits;
			return *this;
		}

		Person& setKills(Int32 kills)
		{
			this->kills = kills;
			return *this;
		}

		Person& setWins(Int32 wins)
		{
			this->wins = wins;
			return *this;
		}

		Person& setGames(Int32 games)
		{
			this->games = games;
			return *this;
		}

		Person& reset();

		void serialize(File& file) const;
		void deSerialize(File& file);
	};
}

#endif