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

#ifndef DUEL6_GUI_CONTROL_H
#define DUEL6_GUI_CONTROL_H

#include <SDL2/SDL_keycode.h>
#include "../Font.h"
#include "../Color.h"
#include "../SysEvent.h"

namespace Duel6 {
    namespace Gui {
        class View;

        class Control {
            friend class View;

        public:
            enum class Type {
                Button,
                Checkbox,
                Label,
                Textbox,
                Listbox,
                Switchbox,
                Slider,
                DialogTitlebar,
                Image
            };

        protected:
            Int32 x = 0, y = 0;
            View *parent = nullptr;
            bool focusable = false;
            bool focused = false;
            bool enabled = true;
        public:
            void setFocusable(bool value);

            void setParent(View *desktop);

            Control(View &parentView);

            virtual ~Control() {
            }

            virtual Type getType() const = 0;

            Int32 getX() const {
                return x;
            }

            Int32 getY() const {
                return y;
            }

            bool isFocused() const {
                return focused;
            }

            void setFocused(bool val) {
                focused = val;
            }

            void setEnabled(bool val){
                enabled = val;
            }
        protected:
            virtual void update(Float32 elapsedTime) {
            }

            virtual void drawStencil(Renderer &renderer, const Font &font) const {}

            virtual void draw(Renderer &renderer, const Font &font) const = 0;

            virtual void drawWithStencilTest(Renderer &renderer, const Font &font) {}

            virtual bool keyEvent(const KeyPressEvent &event) {
                return false;
            }

            virtual void textInputEvent(const TextInputEvent &event) {
            }

            virtual void mouseMotionEvent(const MouseMotionEvent &event) {
            }

            virtual void mouseButtonEvent(const MouseButtonEvent &event) {
            }

            virtual void mouseWheelEvent(const MouseWheelEvent &event) {
            }

            virtual void onBlur() {
            }

            void focus();

            void focusNext();

            void focusPrevious();

            void setIBeamCursor();

            Font& getFont();

        protected:
            static void drawFrame(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h, bool p, bool focus = false);

            static void drawFocusFrame(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h, Float32 lineWidth = 2.0f);

            static bool mouseIn(const MouseEvent &event, Int32 x, Int32 y, Int32 w, Int32 h) {
                return event.getX() >= x && event.getX() < x + w && event.getY() <= y && event.getY() > y - h;
            }
        };
    }
}

#endif
