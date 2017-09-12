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
#include "../Video.h"
#include <algorithm>
#include "Slider.h"

namespace Duel6 {
    namespace Gui {
        namespace {
            Color bcgColor(192, 192, 192);
        }

        Slider::Slider(Desktop &desk)
                : Control(desk) {
            up = new Button(desk);
            up->setCaption(" ");
            down = new Button(desk);
            down->setCaption(" ");
            pos = nullptr;
        }

        void Slider::setPosition(int X, int Y, int H) {
            up->setPosition(X, Y, 16, 16);
            down->setPosition(X, Y - H + 16, 16, 16);
            x = X;
            y = Y - 17;
            height = H - 32;
        }

        void Slider::connect(Slider::Position *to) {
            pos = to;
        }

        void Slider::update(Float32 elapsedTime) {
            if (!up->isPressed() && !down->isPressed()) {
                repeatWait = 0;
            } else {
                if (repeatWait <= 0.0f) {
                    if (up->isPressed() && pos->start > 0) {
                        pos->start--;
                        repeatWait = 0.1f;
                    }
                    if (down->isPressed() && pos->start < pos->items - pos->showCount) {
                        pos->start++;
                        repeatWait = 0.1f;
                    }
                } else {
                    repeatWait -= elapsedTime;
                }
            }
        }

        void Slider::mouseButtonEvent(const MouseButtonEvent &event) {
            if (Control::mouseIn(event, x, y, 16, height) && event.getButton() == SysEvent::MouseButton::LEFT &&
                event.isPressed()) {
                scroll(event.getY());
            }
        }

        void Slider::mouseMotionEvent(const MouseMotionEvent &event) {
            if (Control::mouseIn(event, x, y, 16, height) && event.isPressed(SysEvent::MouseButton::LEFT)) {
                scroll(event.getY());
            }
        }

        void Slider::scroll(Int32 mouseY) {
            Int32 oldStart = pos->start;

            Int32 offset = y - getSliderHeight() / 2 - mouseY;
            pos->start = offset * pos->items / height;

            if (pos->start > pos->items - pos->showCount) {
                pos->start = pos->items - pos->showCount;
            }
            if (pos->start < 0) {
                pos->start = 0;
            }

            if (pos->start != oldStart) {
                // EventType::Change;
            }
        }

        void Slider::draw(const Font &font) const {
            int px, py, h = getSliderHeight(), s = 0;

            px = up->getX() + 7 + (up->isPressed() ? 1 : 0);
            py = up->getY() - 4 - (up->isPressed() ? 1 : 0);
            globRenderer->triangle(Vector(px, py), Vector(px + 2, py - 6), Vector(px - 3, py - 6), Color::BLACK);


            px = down->getX() + 7 + (down->isPressed() ? 1 : 0);
            py = down->getY() - 4 + (down->isPressed() ? 1 : 0);
            globRenderer->triangle(Vector(px - 3, py), Vector(px + 3, py), Vector(px, py - 6), Color::BLACK);

            globRenderer->quadXY(Vector(x, y - height + 1), Vector(15, height - 1), Color(230, 230, 230));

            if (pos->items > 0) {
                s = pos->start * height / pos->items;
                if (s > height - h)
                    s = height - h;
            }

            globRenderer->quadXY(Vector(x, y - s - h + 1), Vector(15, h - 1), bcgColor);

            drawFrame(x, y - s, 16, h, false);
        }

        Int32 Slider::getSliderHeight() const {
            if (pos->items > 0) {
                return std::max(std::min(pos->showCount * height / pos->items, height), 5);
            } else {
                return height;
            }
        }
    }
}
