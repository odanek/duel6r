/*
 * ChatInput.cpp
 *
 *  Created on: Oct 17, 2020
 *      Author: fanick
 */

#include "ChatInput.h"
#include "renderer/Renderer.h"
#include "Font.h"
#include "math/Vector.h"
#include "SysEvent.h"
#include "input/Input.h"
namespace Duel6 {

    ChatInput::ChatInput(Input &input):input(input){

    }

    void ChatInput::render(Renderer &renderer, const Font &font) const {
        Int32 x = 20;
        Int32 y = 20;
        const std::string global = "GLOBAL:";
        auto globalWidth = font.getTextWidth(global, 16);
        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        renderer.quadXY(Vector(x, y + 1), Vector( globalWidth + font.getTextWidth(text, 16) + 20, 14), Color(0, 0, 255, 178));
        renderer.setBlendFunc(BlendFunc::None);
        font.print(x, y, Color::WHITE, global, true);
        font.print(x + globalWidth + 5, y, Color::WHITE, text, true);
    }
    void ChatInput::textInputEvent(const std::string &s) {
        text.append(s);
    }

    void ChatInput::keyEvent(const KeyPressEvent &event) {
        if (!text.empty() && event.getCode() == SDLK_BACKSPACE) {
            text.pop_back();
        }
    }

    bool ChatInput::toggle() {
        visible = !visible;
        input.setLocked(visible);
        return visible;
    }

} /* namespace Duel6 */
