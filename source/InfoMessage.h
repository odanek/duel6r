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

#ifndef DUEL6_INFOMESSAGE_H
#define DUEL6_INFOMESSAGE_H

#include <string>
#include "Player.h"

namespace Duel6
{
	class InfoMessage
	{
	public:
		const Player* m_player;
		std::string m_text;
		float m_remainingTime;

	public:
		InfoMessage(const Player& player, const std::string& text, float duration)
			: m_player(&player), m_text(text), m_remainingTime(duration)
		{}

		InfoMessage(const InfoMessage& msg)
			: m_player(msg.m_player), m_text(msg.m_text), m_remainingTime(msg.m_remainingTime)
		{}

		InfoMessage& operator=(const InfoMessage& msg)
		{
			m_player = msg.m_player;
			m_text = msg.m_text;
			m_remainingTime = msg.m_remainingTime;
			return *this;
		}

		const Player& Player() const
		{
			return *m_player;
		}

		const std::string& Text() const
		{
			return m_text;
		}

		float RemainingTime() const
		{
			return m_remainingTime;
		}

		InfoMessage& UpdateRemainingTime(float elapsedTime)
		{
			m_remainingTime -= elapsedTime;
			return *this;
		}
	};
}

#endif