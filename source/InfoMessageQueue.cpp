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

#include "project.h"
#include "InfoMessageQueue.h"

#define D6_INFO_DURATION        5

namespace Duel6
{
	InfoMessageQueue& InfoMessageQueue::Add(const Player& player, const char *formatString, ...)
	{
		char formattedText[100];
		va_list argptr;

		va_start(argptr, formatString);
		vsprintf(formattedText, formatString, argptr);
		va_end(argptr);

		m_messages.push_back(InfoMessage(player, formattedText, D6_INFO_DURATION));
		return *this;
	}

	InfoMessageQueue& InfoMessageQueue::Update(float elapsedTime)
	{
		for (InfoMessage& msg : m_messages)
		{
			msg.UpdateRemainingTime(elapsedTime);
		}

		while (!m_messages.empty() && m_messages.front().RemainingTime() < 0)
		{
			m_messages.pop_front();
		}

		return *this;
	}

	void InfoMessageQueue::RenderPlayerMessages(const Player& player) const
	{
		const d6VIEW_s& view = player.View;
		int posX = view.X + 4;
		int posY = view.Y + view.Height - 24;

		CO_FontColor(255, 255, 0);

		for (const InfoMessage& msg : m_messages)
		{
			if (&player == &msg.Player())
			{
				RenderMessage(posX, posY, msg.Text());
				posY -= 16;
			}
		}
	}

	void InfoMessageQueue::RenderAllMessages(const d6VIEW_s& view) const
	{
		int posX = view.X + 4;
		int posY = view.Y + view.Height - (d6Playing > 4 ? 50 : 20);

		CO_FontColor(255, 255, 0);

		for (const InfoMessage& msg : m_messages)
		{
			RenderMessage(posX, posY, msg.Player().Person().Name() + ": " + msg.Text());
			posY -= 16;
		}
	}

	void InfoMessageQueue::RenderMessage(int x, int y, const std::string& msg)
	{
		glColor4f(0, 0, 1, 0.7f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);
			glVertex2i(x, y + 15);
			glVertex2i(x + 8 * msg.length(), y + 15);
			glVertex2i(x + 8 * msg.length(), y + 1);
			glVertex2i(x, y + 1);
		glEnd();
		glDisable(GL_BLEND);

		CO_FontPrintf(x, y, msg.c_str());
	}

}