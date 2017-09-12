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
#include "Spinner.h"

namespace Duel6 {
    namespace Gui {
        Spinner::Spinner(Desktop &desk)
                : Control(desk) {
            now = -1;

            left = new Button(desk);
            left->setCaption(" ");
            right = new Button(desk);
            right->setCaption(" ");
        }

        Spinner::~Spinner() {
            clear();
        }

        void Spinner::clear() {
            items.clear();
            now = -1;
        }

        Int32 Spinner::currentItem() {
            return now;
        }

        void Spinner::setCurrent(Int32 n) {
            now = n;
        }

        void Spinner::removeItem(Int32 n) {
            if (n < 0 || n >= (Int32) items.size())
                return;

            items.erase(items.begin() + n);
            if (now >= (Int32) items.size())
                now = Int32(items.size()) - 1;
        }

        void Spinner::addItem(const std::string &item) {
            items.push_back(item);
            if (items.size() == 1) {
                now = 0;
            }
        }

        void Spinner::setPosition(Int32 X, Int32 Y, Int32 W, Int32 H) {
            left->setPosition(X, Y - 1, 18, 18);
            right->setPosition(X + W - 18, Y - 1, 18, 18);
            width = W;
            x = X;
            y = Y;
        }

        void Spinner::update(Float32 elapsedTime) {
            if (!left->isPressed() && !right->isPressed()) {
                repeatWait = 0.0f;
            } else {
                if (repeatWait > 0.0f) {
                    repeatWait -= elapsedTime;
                } else {
                    if (left->isPressed() && now > 0) {
                        repeatWait = 0.3f;
                        now--;
                    }
                    if (right->isPressed() && now + 1 < (Int32) items.size()) {
                        repeatWait = 0.3f;
                        now++;
                    }
                }
            }
        }

        void Spinner::draw(const Font &font) const {
            int px, py;

            drawFrame(x + 20, y, width - 40, 18, true);
            globRenderer->quadXY(Vector(x + 22, y - 16), Vector(width - 44, 15), Color::WHITE);

            px = left->getX() + 7 + (left->isPressed() ? 1 : 0);
            py = left->getY() - 4 - (left->isPressed() ? 1 : 0);
            globRenderer->triangle(Vector(px + 2, py), Vector(px + 2, py - 7), Vector(px - 2, py - 4), Color::BLACK);

            px = right->getX() + 7 + (right->isPressed() ? 1 : 0);
            py = right->getY() - 4 - (right->isPressed() ? 1 : 0);
            globRenderer->triangle(Vector(px - 1, py), Vector(px - 1, py - 7), Vector(px + 3, py - 4), Color::BLACK);

            if (items.empty())
                return;

            font.print(x + 25, y - 15, Color(0), items[now]);
        }
    }
}
