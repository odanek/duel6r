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

#ifndef DUEL6_SYSEVENT_H
#define DUEL6_SYSEVENT_H

#include <SDL2/SDL.h>
#include <string>
#include "Type.h"

namespace Duel6 {
    class SysEvent {
    public:
        enum class MouseButton {
            NONE = 0x00,
            LEFT = SDL_BUTTON_LMASK,
            MIDDLE = SDL_BUTTON_MMASK,
            RIGHT = SDL_BUTTON_RMASK
        };

        enum class ButtonState {
            PRESSED,
            RELEASED
        };

        enum class KeyModifier {
            ALT = KMOD_ALT,
            CTRL = KMOD_CTRL,
            SHIFT = KMOD_SHIFT
        };

    public:
        virtual ~SysEvent() {}
    };

    class MouseEvent : public SysEvent {
    protected:
        Int32 x;
        Int32 y;

    public:
        MouseEvent(Int32 x, Int32 y)
                : x(x), y(y) {}

        Int32 getX() const {
            return x;
        }

        Int32 getY() const {
            return y;
        }
    };

    class MouseButtonEvent : public MouseEvent {
    private:
        MouseButton button;
        ButtonState state;
        bool doubleClick;

    public:
        MouseButtonEvent(Int32 x, Int32 y, MouseButton button, ButtonState state, bool doubleClick)
                : MouseEvent(x, y), button(button), state(state), doubleClick(doubleClick) {}

        const MouseButton &getButton() const {
            return button;
        }

        bool isPressed() const {
            return state == ButtonState::PRESSED;
        }

        bool isDoubleClick() const {
            return doubleClick;
        }

        MouseButtonEvent translate(Int32 tx, Int32 ty) const {
            return MouseButtonEvent(x + tx, y + ty, button, state, doubleClick);
        }
    };

    class MouseMotionEvent : public MouseEvent {
    private:
        Int32 xDiff;
        Int32 yDiff;
        Uint32 buttonState;

    public:
        MouseMotionEvent(Int32 x, Int32 y, Int32 xDiff, Int32 yDiff, Uint32 buttonState)
                : MouseEvent(x, y), xDiff(xDiff), yDiff(yDiff), buttonState(buttonState) {}

        Int32 getXDiff() const {
            return xDiff;
        }

        Int32 getYDiff() const {
            return yDiff;
        }

        bool isPressed(MouseButton button) const {
            return (buttonState & (Uint32) button) == (Uint32) button;
        }

        MouseMotionEvent translate(Int32 tx, Int32 ty) const {
            return MouseMotionEvent(x + tx, y + ty, xDiff, yDiff, buttonState);
        }
    };

    class MouseWheelEvent : public MouseEvent {
    private:
        Int32 amountX;
        Int32 amountY;

    public:
        MouseWheelEvent(Int32 x, Int32 y, Int32 amountX, Int32 amountY)
                : MouseEvent(x, y), amountX(amountX), amountY(amountY) {}

        Int32 getAmountX() const {
            return amountX;
        }

        Int32 getAmountY() const {
            return amountY;
        }
    };

    class KeyPressEvent : public SysEvent {
    private:
        SDL_Keycode code;
        ButtonState state;
        Uint16 modifiers;

    public:
        KeyPressEvent(SDL_Keycode code, const ButtonState &state, Uint16 modifiers)
                : code(code), state(state), modifiers(modifiers) {}

        SDL_Keycode getCode() const {
            return code;
        }

        ButtonState getState() const {
            return state;
        }

        bool isPressed() const {
            return state == ButtonState::PRESSED;
        }

        Uint16 getModifiers() const {
            return modifiers;
        }

        bool hasModifier(KeyModifier modifier) const {
            return (modifiers & (Uint16) modifier) != 0;
        }

        bool withShift() const {
            return hasModifier(KeyModifier::SHIFT);
        }

        bool withAlt() const {
            return hasModifier(KeyModifier::ALT);
        }

        bool withCtrl() const {
            return hasModifier(KeyModifier::CTRL);
        }
    };

    class TextInputEvent : public SysEvent {
    private:
        const std::string &text;

    public:
        TextInputEvent(const std::string &text)
                : text(text) {}

        const std::string &getText() const {
            return text;
        }
    };
}

#endif
