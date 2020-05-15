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

#include "CheckBox.h"
#include "Desktop.h"

namespace Duel6 {
    namespace Gui {
    CheckBox::CheckBox(Desktop &desk, bool value)
                : Control(desk) {
            checked = value;
            focusable = true;
        }

        void CheckBox::setLabel(const std::string &label) {
            this->label = label;
        }

        void CheckBox::setPosition(int X, int Y, int W, int H) {
            x = X;
            y = Y;
            width = W;
            height = H;
        }

        void CheckBox::mouseButtonEvent(const MouseButtonEvent &event) {
            if (Control::mouseIn(event, x, y, width, height)) {
                if (event.getButton() == SysEvent::MouseButton::LEFT) {
                    if (event.isPressed()) {
                        this->parent->focus(this);
                        toggle();
                    }
                }
            }
        }

        void CheckBox::draw(Renderer &renderer, const Font &font) const {
            Int32 px, py;

            drawFrame(renderer, x, y, 16, 16, checked, focused);
            px = x + 32;
            py = y - (height >> 1) - 7;
            font.print(px, py, Color::BLACK, label);
        }

        bool CheckBox::keyEvent(const KeyPressEvent &event) {
            switch (event.getCode()) {
            case (SDLK_RETURN):
            case (SDLK_KP_ENTER):
            case (SDLK_SPACE):
            case (SDLK_KP_SPACE): {
                toggle();
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
    }
}
