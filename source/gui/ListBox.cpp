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
#include "ListBox.h"
#include "../Video.h"
#include "Desktop.h"

namespace Duel6 {
    namespace {
        Color highlightColor(0, 0, 200);
    }

    namespace Gui {
        ListBox::ListBox(Desktop &desk, bool sb)
                : Control(desk) {
            focusable = true;
            listPos.items = 0;
            listPos.start = 0;
            selected = -1;
            scrollBar = sb;
            colorizeCallback = defaultColorize;
            if (sb) {
                slider = new Slider(desk);
                slider->connect(&listPos);
            }
        }

        ListBox &ListBox::clear() {
            items.clear();
            listPos.items = 0;
            listPos.start = 0;
            selected = -1;
            return *this;
        }

        Int32 ListBox::selectedIndex() const {
            return selected;
        }

        const std::string &ListBox::selectedItem() const {
            return items[selected];
        }

        ListBox &ListBox::selectItem(Int32 index) {
            if(index < 0 || index >= items.size()) return *this;
            if (index != selected) {
                selected = index;
                for (auto &listener : selectListeners) {
                    listener(index, items[index]);
                }
            }
            return *this;
        }

        ListBox &ListBox::scrollToView(Int32 index) {
//            if(index > listPos.start && index < listPos.start + listPos.showCount){
//                return *this;
//            }

            if(index > listPos.start + listPos.showCount){
                listPos.start += index - selected;
            } else
                if (index < listPos.start) {
                    listPos.start -= selected - index;
                }
            if(listPos.start < 0) {
                listPos.start = 0;
            }
            return *this;
        }

        ListBox &ListBox::removeItem(Int32 index) {
            if (index >= 0 && index < listPos.items) {
                items.erase(items.begin() + index);
                listPos.items--;
                if (selected >= listPos.items) {
                    selected = listPos.items - 1;
                }
            }
            return *this;
        }

        ListBox &ListBox::removeItem(const std::string &item) {
            auto iter = std::find(items.begin(), items.end(), item);
            if (iter != items.end()) {
                removeItem((Int32) (iter - items.begin()));
            }
            return *this;
        }

        ListBox &ListBox::addItem(const std::string &item) {
            listPos.items++;
            items.push_back(item);
            return *this;
        }

        const std::string &ListBox::getItem(Size index) const {
            return items.at(index);
        }

        Size ListBox::size() const {
            return items.size();
        }

        ListBox &ListBox::setPosition(Int32 x, Int32 y, Int32 width, Int32 height, Int32 itemHeight) {
            this->x = x + 2;
            this->y = y - 2;
            this->width = width << 3;
            this->itemHeight = itemHeight;
            this->height = height * itemHeight;
            listPos.showCount = height;
            if (scrollBar) {
                slider->setPosition(x + (width << 3) + 4, y, height * itemHeight + 4);
            }
            return *this;
        }

        void ListBox::mouseButtonEvent(const MouseButtonEvent &event) {
            if (items.size() > 0 && Control::mouseIn(event, x, y, width, height) &&
                event.getButton() == SysEvent::MouseButton::LEFT && event.isPressed()) {
                this->parent->focus(this);
                Int32 itemIndex = std::max(listPos.start + ((y - event.getY()) / itemHeight), 0);

                if (itemIndex >= listPos.items) {
                    selectItem(-1);
                } else {
                    selectItem(itemIndex);
                    if (event.isDoubleClick()) {
                        for (auto &listener : doubleClickListeners) {
                            listener(itemIndex, items[itemIndex]);
                        }
                    }
                }
            }
        }

        void ListBox::mouseWheelEvent(const MouseWheelEvent &event) {
            if (items.size() > 0 && Control::mouseIn(event, x, y, width, height)) {
                Int32 itemIndex = std::min(listPos.start - 3 * event.getAmountY(), (Int32) items.size() - listPos.showCount);
                if (itemIndex < 0) {
                    itemIndex = 0;
                }
                listPos.start = itemIndex;
            }
        }
        void ListBox::keyEvent(const KeyPressEvent &event) {
            switch (event.getCode()) {
            case (SDLK_UP): {
                selectItem(selected - 1);
                scrollToView(selected - 1);
                break;
            }
            case (SDLK_DOWN): {
                selectItem(selected + 1);
                scrollToView(selected + 1);
                break;
            }
            case (SDLK_RETURN):
                case (SDLK_KP_ENTER):
                case (SDLK_SPACE):
                case (SDLK_KP_SPACE): {
                for (auto &listener : doubleClickListeners) {
                    listener(selected, items[selected]);
                }
                break;
            }
            }
        }

        void ListBox::draw(Renderer &renderer, const Font &font) const {
            drawFrame(renderer, x - 2, y + 2, width + 4, height + 4, true, focused);
            renderer.quadXY(Vector(x, y - height + 1), Vector(width, height - 1), Color::WHITE);

            if (items.empty())
                return;

            Int32 Y = y;
            Int32 shift = 15 + (itemHeight - 16) / 2;

            Int32 firstIndex = std::max(listPos.start, 0);
            Int32 visibleCount = listPos.showCount;

            for (Int32 index = firstIndex; index < firstIndex + visibleCount; index++, Y -= itemHeight) {
                if (index >= listPos.items)
                    break;

                const std::string& label = items[index];
                ItemColor colors = colorizeCallback(index, label);

                Color fontColor = (index == selected) ? Color::WHITE : colors.font;
                Color bcgColor = (index == selected) ? highlightColor : colors.background;

                renderer.quadXY(Vector(x, Y - (itemHeight - 1)), Vector(width - 1, itemHeight - 1), bcgColor);
                font.print(x, Y - shift, fontColor, label.substr(0, size_t(width) / 8));
            }
        }

        ListBox::ItemColor ListBox::defaultColorize(Int32 index, const std::string& label) {
            return {Color::BLACK, Color::WHITE};
        }
    }
}
