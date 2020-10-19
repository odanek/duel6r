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
#include "Game.h"
#include "Font.h"
#include "ChatMessageQueue.h"
#if defined(D6_RENDERER_HEADLESS)
#include <iostream>
#endif
namespace Duel6 {
    ChatMessageQueue::ChatMessageQueue(Float32 duration)
        : duration(duration) {
    }

    ChatMessageQueue& ChatMessageQueue::add(bool system, const std::string &origin, const std::string &msg) {
#if defined(D6_RENDERER_HEADLESS)
        std::cout << std::string( system ? "[SYSTEM] " : "") << origin << ":" << msg << std::endl;
#endif
        messages.push_back(ChatMessage(msg, duration, origin, system));
        return *this;
    }

    ChatMessageQueue& ChatMessageQueue::update(float elapsedTime) {
        for (ChatMessage &msg : messages) {
            msg.updateRemainingTime(elapsedTime);
        }

        while (!messages.empty() && messages.front().getRemainingTime() < 0) {
            messages.pop_front();
        }

        return *this;
    }

    void ChatMessageQueue::renderAllMessages(Renderer &renderer, const Int32 height, Int32 offsetY, const Font &font) const {
        Int32 posX = 4;
        Int32 posY = offsetY;
        const std::string system("[SYSTEM] ");
        for (auto it = messages.rbegin(); it != messages.rend(); it++){
            renderMessage(renderer, posX, posY, (it->isSystem() ? system : "") + it->getOrigin() + ": " + it->getText(), font);
            posY += 16;

        }
    }

    void ChatMessageQueue::renderMessage(Renderer &renderer, Int32 x, Int32 y, const std::string &msg, const Font &font) {
        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        renderer.quadXY(Vector(x, y + 1), Vector(font.getTextWidth(msg, 16), 14), Color(0, 0, 255, 100));
        renderer.setBlendFunc(BlendFunc::None);
        font.print(x, y, Color::CYAN, msg, true);
    }

    void ChatMessageQueue::clear() {
        messages.clear();
    }
}
