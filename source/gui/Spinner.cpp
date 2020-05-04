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
#include <algorithm>
#include <utility>
#include "../Video.h"
#include "Spinner.h"
#include "Desktop.h"
namespace Duel6 {
    namespace Gui {
        Spinner::Spinner(Desktop &desk)
                : Control(desk) {
            selectedIndex = -1;
            focusable = true;
            left = new Button(desk);
            left->setCaption(" ");
            right = new Button(desk);
            right->setCaption(" ");
            left->setFocusable(false);
            right->setFocusable(false);
        }

        void Spinner::clear() {
            items.clear();
            setCurrent(-1);
        }

        Int32 Spinner::currentItem() {
            return selectedIndex;
        }
        std::pair<Int32, std::string> & Spinner::currentValue() {
            return items[selectedIndex];
        }
        void Spinner::setCurrent(Int32 index) {
            Int32 itemCount = items.size();
            if ((index >= 0 && index < itemCount) || (index == -1 && itemCount == 0)) {
                selectedIndex = index;
                for (auto &listener : toggleListeners) {
                    listener(selectedIndex);
                }
            }
        }

        void Spinner::removeItem(Int32 n) {
            Int32 itemCount = items.size();
            if (n < 0 || n >= itemCount)
                return;

            items.erase(items.begin() + n);
            itemCount--;

            if (selectedIndex >= itemCount)
                setCurrent(itemCount - 1);
        }

        void Spinner::addItem(const std::string &item, int value, bool skipIfPresent) {
            if(!skipIfPresent || (std::find_if(items.begin(), items.end(), [item](std::pair<int, std::string> & p){return p.second == item;}) == std::end(items))){
                items.push_back(std::make_pair(value, item));
            }
            if (items.size() == 1) {
                setCurrent(0);
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
                    if (left->isPressed()) {
                        repeatWait = 0.3f;
                        setCurrent(selectedIndex - 1);
                    }
                    if (right->isPressed()) {
                        repeatWait = 0.3f;
                        setCurrent(selectedIndex + 1);
                    }
                }
            }
        }

        void Spinner::draw(Renderer &renderer, const Font &font) const {
            drawFrame(renderer, x + 20, y, width - 40, 18, true);
            renderer.quadXY(Vector(x + 22, y - 16), Vector(width - 44, 15), Color::WHITE);
            Int32 px = left->getX() + 7 + (left->isPressed() ? 1 : 0);
            Int32 py = left->getY() - 4 - (left->isPressed() ? 1 : 0);
            renderer.triangle(Vector(px + 2, py), Vector(px + 2, py - 7), Vector(px - 2, py - 4), Color::BLACK);

            px = right->getX() + 7 + (right->isPressed() ? 1 : 0);
            py = right->getY() - 4 - (right->isPressed() ? 1 : 0);
            renderer.triangle(Vector(px - 1, py), Vector(px + 3, py - 4), Vector(px - 1, py - 7), Color::BLACK);

            if (items.empty())
                return;
            ItemColor colors = colorizeCallback(selectedIndex, items[selectedIndex].second);

            Color fontColor = colors.font;
            Color bcgColor =  colors.background;

            renderer.quadXY(Vector(x + 22, y - 16), Vector(width - 44, 15),bcgColor);
            font.print(x + 25, y - 15, fontColor, items[selectedIndex].second);
            if(focused){
                drawFocusFrame(renderer, x-1,y-1, width, 18);
            }
        }
        void Spinner::mouseButtonEvent(const MouseButtonEvent &event){
            if (Control::mouseIn(event, x, y, width, 20)) {
                this->parent->focus(this);
            }
        }
        void Spinner::keyEvent(const KeyPressEvent &event) {
            switch (event.getCode()) {
                case (SDLK_LEFT): {
                    setCurrent(selectedIndex - 1);
                    break;
                }
                case (SDLK_RIGHT): {
                    setCurrent(selectedIndex + 1);
                    break;
                }
                case (SDLK_UP): {
                    parent->focusPrevious();
                    break;
                }
                case (SDLK_DOWN): {
                    parent->focusNext();
                    break;
                }
            }
        }

        Spinner::ItemColor Spinner::defaultColorize(Int32 index, const std::string& item) {
            return {Color::BLACK, Color::WHITE};
        }
    }
}
