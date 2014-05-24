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


#include "core/co_core.h"
#include "InfoMessageQueue.h"

namespace Duel6
{
	InfoMessageQueue& InfoMessageQueue::add(const Player& player, const char *formatString, ...)
	{
		char formattedText[100];
		va_list argptr;

		va_start(argptr, formatString);
		vsprintf(formattedText, formatString, argptr);
		va_end(argptr);

		messages.push_back(InfoMessage(player, formattedText, duration));
		return *this;
	}

	InfoMessageQueue& InfoMessageQueue::update(float elapsedTime)
	{
		for (InfoMessage& msg : messages)
		{
			msg.updateRemainingTime(elapsedTime);
		}

		while (!messages.empty() && messages.front().getRemainingTime() < 0)
		{
			messages.pop_front();
		}

		return *this;
	}

	void InfoMessageQueue::renderPlayerMessages(const Player& player) const
	{
		const PlayerView& view = player.getView();
		Int32 posX = view.getX() + 4;
		Int32 posY = view.getY() + view.getHeight() - 24;

		CO_FontColor(255, 255, 0);

		for (const InfoMessage& msg : messages)
		{
			if (player.is(msg.getPlayer()))
			{
				renderMessage(posX, posY, msg.getText());
				posY -= 16;
			}
		}
	}

	void InfoMessageQueue::renderAllMessages(const PlayerView& view, Int32 offsetY) const
	{
		Int32 posX = view.getX() + 4;
		Int32 posY = view.getY() + view.getHeight() - offsetY;

		CO_FontColor(255, 255, 0);

		for (const InfoMessage& msg : messages)
		{
			renderMessage(posX, posY, msg.getPlayer().getPerson().getName() + ": " + msg.getText());
			posY -= 16;
		}
	}

	void InfoMessageQueue::renderMessage(Int32 x, Int32 y, const std::string& msg)
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