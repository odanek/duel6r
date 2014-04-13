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

namespace Duel6
{
	class Person
	{
	private:
		std::string m_name;
		Int32 m_shots;
		Int32 m_hits;
		Int32 m_kills;
		Int32 m_wins;
		Int32 m_games;

	public:
		Person()
			: m_shots(0), m_hits(0), m_kills(0), m_wins(0), m_games(0)
		{}

		explicit Person(const std::string& name)
			: Person()
		{
			m_name = name;
		}

		const std::string& Name() const
		{
			return m_name;
		}

		Int32 Shots() const
		{
			return m_shots;
		}

		Int32 Hits() const
		{
			return m_hits;
		}

		Int32 Kills() const
		{
			return m_kills;
		}

		Int32 Wins() const
		{
			return m_wins;
		}

		Int32 Games() const
		{
			return m_games;
		}

		Int32 TotalPoints() const
		{
			return Kills() + Wins();
		}

		Person& SetShots(Int32 shots)
		{
			m_shots = shots;
			return *this;
		}

		Person& SetHits(Int32 hits)
		{
			m_hits = hits;
			return *this;
		}

		Person& SetKills(Int32 kills)
		{
			m_kills = kills;
			return *this;
		}

		Person& SetWins(Int32 wins)
		{
			m_wins = wins;
			return *this;
		}

		Person& SetGames(Int32 games)
		{
			m_games = games;
			return *this;
		}

		Person& Reset();

		void Serialize(FILE* file) const;
		void DeSerialize(FILE* file);
	};
}

#endif