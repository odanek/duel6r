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

#ifndef DUEL6_GUI_LISTBOX_H
#define DUEL6_GUI_LISTBOX_H

#include <vector>
#include "Control.h"
#include "Slider.h"

namespace Duel6 {
    namespace Gui {
        class ListBox : public Control {
        public:
            struct ItemColor {
                Color font;
                Color background;
            };

            typedef std::function<void(Int32 index, const std::string &item)> ClickCallback;
            typedef std::function<ItemColor(Int32 index, const std::string &label)> ColorizeCallback;

        private:
            std::vector<ClickCallback> selectListeners;
            std::vector<ClickCallback> doubleClickListeners;
            ColorizeCallback colorizeCallback;
            bool scrollBar;
            Slider *slider;
            Int32 width = 0;
            Int32 height = 0;
            Int32 selected;
            Int32 itemHeight = 1;
            std::vector<std::string> items;
            Slider::Position listPos;

        public:
            ListBox(View &parentView, bool sb);

            ListBox& setPosition(Int32 x, Int32 y, Int32 width, Int32 height, Int32 itemHeight);

            ListBox& addItem(const std::string &item);

            ListBox& removeItem(Int32 index);

            ListBox& removeItem(const std::string &item);

            ListBox& selectItem(Int32 index);

            ListBox& scrollToView(Int32 index);

            const std::string& getItem(Size index) const;

            Int32 selectedIndex() const;

            const std::string& selectedItem() const;

            Size size() const;

            ListBox& clear();

            Control::Type getType() const override {
                return Control::Type::Listbox;
            }

            ListBox& onItemSelected(ClickCallback listener) {
                selectListeners.push_back(listener);
                return *this;
            }

            ListBox& onDoubleClick(ClickCallback listener) {
                doubleClickListeners.push_back(listener);
                return *this;
            }

            ListBox& onColorize(ColorizeCallback callback) {
                colorizeCallback = callback;
                return *this;
            }

            static ItemColor defaultColorize(Int32 index, const std::string &label);

        protected:
            void mouseWheelEvent(const MouseWheelEvent &event) override;

            void mouseButtonEvent(const MouseButtonEvent &event) override;

            bool keyEvent(const KeyPressEvent &event) override;

            void drawStencil(Renderer &renderer, const Font &font) const override;

            void draw(Renderer &renderer, const Font &font) const override;

            void drawWithStencilTest(Renderer &renderer, const Font &font) override;
        };
    }
}

#endif
