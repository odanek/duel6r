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

#ifndef DUEL6_GUI_TEXTBOX_H
#define DUEL6_GUI_TEXTBOX_H

#include <string>
#include "Control.h"

namespace Duel6 {
    namespace Gui {
        class Textbox : public Control {

        public:
            typedef std::function<void(Textbox &textBox)> OnEnterCallback;

        private:
            std::vector<OnEnterCallback> onEnterListeners;
            Int32 max, width;
            const Int32 height = 18;
            std::string text;
            std::string confirmedText;
            std::string allowedCharacters;
            Int32 cursorPos = 0;
            Float32 blinkCountDown = 0.5f;
            std::string placeholder = "";
        public:
            Textbox(Desktop &desk);

            ~Textbox();

            void setPosition(int X, int Y, int W, int M, const std::string &allowed);

            void setText(const std::string & value);

            void setPlaceholder(const std::string &placeholder);

            const std::string &getText() const;

            void flush();

            void draw(Renderer &renderer, const Font &font) const override;

            void textInputEvent(const TextInputEvent &event) override;

            bool keyEvent(const KeyPressEvent &event) override;

            Control::Type getType() const override {
                return Control::Type::Textbox;
            }
            Textbox &onEnter(OnEnterCallback listener) {
                onEnterListeners.push_back(listener);
                return *this;
            }

            void update(Float32 elapsedTime) override;

        protected:
            void mouseButtonEvent(const Duel6::MouseButtonEvent &event) override;

            void onBlur() override;

            void mouseMotionEvent(const MouseMotionEvent &event) override;
        private:
            void confirmValue();

            void fireOnEnterListeners();

            void moveCursorRight();

            void moveCursorLeft();
        };
    }
}

#endif
