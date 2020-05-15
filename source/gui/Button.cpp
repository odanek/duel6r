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

#include "Button.h"
#include "Desktop.h"

namespace Duel6 {
    namespace Gui {
        Button::Button(Desktop &desk)
                : Control(desk) {
            focusable = true;
            pressed = false;
        }

        Button::~Button() {
        }

        void Button::setCaption(const std::string &caption) {
            this->caption = caption;
        }

        void Button::setPosition(int X, int Y, int W, int H) {
            x = X;
            y = Y;
            width = W;
            height = H;
        }

        bool Button::keyEvent(const KeyPressEvent &event) {
            switch (event.getCode()) {
                case (SDLK_RETURN):
                case (SDLK_KP_ENTER):
                case (SDLK_SPACE):
                case (SDLK_KP_SPACE): {
                    fireClickListeners();
                    firePressListeners(true);
                    return true;
                    break;
                }
            case (SDLK_LEFT):
            case (SDLK_UP): {
                parent->focusPrevious();
                return true;
                break;
            }
            case (SDLK_RIGHT):
            case (SDLK_DOWN): {
                parent->focusNext();
                return true;
                break;
            }
            }
            return false;
        }
        void Button::mouseButtonEvent(const MouseButtonEvent &event) {
            if (Control::mouseIn(event, x, y, width, height)) {
                if (event.getButton() == SysEvent::MouseButton::LEFT) {
                    this->parent->focus(this);
                    if (!pressed && event.isPressed()) {
                        pressed = true;
                        firePressListeners(true);
                    } else if (pressed && !event.isPressed()) {
                        pressed = false;
                        fireClickListeners();
                        firePressListeners(false);
                    }
                }
            }
        }

        void Button::mouseMotionEvent(const MouseMotionEvent &event) {
            if (!Control::mouseIn(event, x, y, width, height) && pressed) {
                pressed = false;
                firePressListeners(false);
            }
        }

        void Button::draw(Renderer &renderer, const Font &font) const {
            Int32 px, py;
            Int32 captionWidth = font.getTextWidth(caption, 16);
            drawFrame(renderer, x, y, width, height, pressed, focused);
            px = x + (width / 2) - (captionWidth / 2) + -1 + pressed;
            py = y - (height / 2) - 8 - pressed;
            font.print(px, py, Color::BLACK, caption);
        }

        void Button::firePressListeners(bool pressed) {
            for (auto &listener : pressListeners) {
                listener(*this, pressed);
            }
        }

        void Button::fireClickListeners() {
            for (auto &listener : clickListeners) {
                listener(*this);
            }
        }
    }
}
