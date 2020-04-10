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

#ifndef DUEL6_GUI_SPINNER_H
#define DUEL6_GUI_SPINNER_H

#include "Control.h"
#include "Button.h"

namespace Duel6 {
    namespace Gui {
        class Spinner
                : public Control {
        public:
            struct ItemColor {
                Color font;
                Color background;
            };

            typedef std::function<void(Int32 selectedIndex)> ToggleCallback;
            typedef std::function<ItemColor(Int32 index, const std::string& label)> ColorizeCallback;

        private:
            std::vector<ToggleCallback> toggleListeners;
            Button *left, *right;
            Int32 selectedIndex;
            Int32 width;
            std::vector<std::pair<Int32, std::string> > items;
            Float32 repeatWait;
            ColorizeCallback colorizeCallback = defaultColorize;

        public:
            explicit Spinner(Desktop &desk);

            void setPosition(Int32 X, Int32 Y, Int32 W, Int32 H);

            void addItem(const std::string &item, Int32 value = 0, bool skipIfPresent = true);

            void removeItem(Int32 n);

            void setCurrent(Int32 index);

            Int32 currentItem();

            std::pair<Int32, std::string> & currentValue();

            void clear();

            Control::Type getType() const override {
                return Control::Type::Switchbox;
            }

            Spinner &onToggled(ToggleCallback listener) {
                toggleListeners.push_back(listener);
                return *this;
            }

            Spinner &onColorize(ColorizeCallback callback) {
                colorizeCallback = callback;
                return *this;
            }
            static ItemColor defaultColorize(Int32 index, const std::string& item);

        protected:
            void mouseButtonEvent(const MouseButtonEvent &event) override;

            void keyEvent(const KeyPressEvent &event) override;

            void update(Float32 elapsedTime) override;

            void draw(Renderer &renderer, const Font &font) const override;
        };

    }
}

#endif
