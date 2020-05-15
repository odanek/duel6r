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
#include <string>
#include <SDL2/SDL_keycode.h>
#include "TextBox.h"
#include "../Video.h"
#include "Desktop.h"
        using std::literals::string_literals::operator ""s;
namespace Duel6 {
    namespace Gui {
        Textbox::Textbox(Desktop &desk)
                : Control(desk) {
            focusable = true;
        }

        Textbox::~Textbox() {
        }

        void Textbox::setPosition(int X, int Y, int W, int M, const std::string &allowed) {
            x = X + 2;
            y = Y - 2;
            max = M;
            width = W;

            allowedCharacters = allowed;
            text.clear();
        }

        void Textbox::moveCursorRight(){
            cursorPos ++;
            if(cursorPos > text.length()){
                cursorPos = text.length();
            }
        }

        void Textbox::moveCursorLeft(){
            cursorPos --;
            if(cursorPos < 0){
                cursorPos = 0;
            }
        }

        bool Textbox::keyEvent(const KeyPressEvent &event) {
            if (event.getCode() == SDLK_ESCAPE) {
                text = confirmedText;
                return false; // don't block the blur event
            }

            if(event.getCode() == SDLK_HOME){
                cursorPos = 0;
                return true;
            }

            if(event.getCode() == SDLK_END){
                cursorPos = text.length();
                return true;
            }

            if(event.getCode() == SDLK_LEFT){
                moveCursorLeft();
                return true;
            }

            if(event.getCode() == SDLK_RIGHT){
                moveCursorRight();
                return true;
            }

            if(event.withCtrl() && event.getCode() == SDLK_v) {
                std::string inserted(SDL_GetClipboardText());
                size_t maxInserted = max - text.length();
                maxInserted = std::min(maxInserted, inserted.length());
                text.insert(cursorPos, inserted, 0, maxInserted);
                cursorPos += maxInserted;
                return true;
            }

            if (!text.empty() && event.getCode() == SDLK_BACKSPACE) {
                if(cursorPos > 0) {
                    text.erase(cursorPos - 1, 1);
                    moveCursorLeft();
                }
                return true;
            }
            if (!text.empty() && event.getCode() == SDLK_DELETE) {
                if(cursorPos < text.length()) {
                    text.erase(cursorPos, 1);
                }
                return true;
            }
            if (!text.empty() && event.getCode() == SDLK_RETURN) {
                fireOnEnterListeners();
                return true;
            }
            return false;
        }

        void Textbox::mouseButtonEvent(const Duel6::MouseButtonEvent &event) {
            int w = (width << 3) + 8;
            if (Control::mouseIn(event, x, y, w, height)) {
                if (event.getButton() == SysEvent::MouseButton::RIGHT) {
                    flush();
                }
                Int32 mx = event.getX();

                cursorPos = 0;
                for(Int32 sx = x + 4; sx < mx  && cursorPos < text.length(); sx += parent->getFont().getTextWidth(text.substr(cursorPos++, 1), 16)){

                }
                parent->focus(this);
            }
        }

        void Textbox::textInputEvent(const TextInputEvent &event) {
            const std::string &newText = event.getText();
            for (auto iter = newText.cbegin(); iter != newText.cend(); ++iter) {
                char letter = *iter;
                if ((int) text.length() < max && allowedCharacters.find(letter) != std::string::npos) {
                    if(cursorPos >= text.length()){
                        text.push_back(letter);
                    } else {
                        text.insert(cursorPos, 1, letter);
                    }
                    moveCursorRight();
                }
            }
        }

        void Textbox::setText(const std::string & value) {
            text = value;
            confirmedText = text;
        }

        const std::string &Textbox::getText() const {
            return text;
        }

        void Textbox::flush() {
            text.clear();
        }

        void Textbox::draw(Renderer &renderer, const Font &font) const {
            int w = (width << 3) + 8;

            drawFrame(renderer, x - 2, y + 2, w + 4, height + 4, true, focused);
            renderer.quadXY(Vector(x, y - (height - 1)), Vector(w, height - 1), Color::WHITE);
            Vector resultSize;
            font.print(x, (y - height), Color::BLACK, text);
            if(text.empty()){
                font.print(x, (y - height) + 4, 0, Color(123,123,123), placeholder, 12);
            }
            if(blinkCountDown > 0.25f){
                std::string prefix = text.substr(0, cursorPos);
                Int32 cursorW = font.getTextWidth(prefix, 32);
                Int32 carretW = font.getTextWidth("|"s, 32);
                if(focused){
                    font.print(x + cursorW / 2 - carretW / 4, y - (height) + 1, 0, Color::BLACK, "|", 18);
                }
            }
        }

        void Textbox::onBlur() {
            confirmValue();
        }

        void Textbox::confirmValue() {
            confirmedText = text;
        }

        void Textbox::fireOnEnterListeners() {
            for (auto &listener : onEnterListeners) {
                listener(*this);
            }
        }
        void Textbox::mouseMotionEvent(const MouseMotionEvent &event) {
            if (Control::mouseIn(event, x, y, (width << 3) + 8, height)) {
                parent->setIBeamCursor();
            }
        }
        void Textbox::update(Float32 elapsedTime) {
            blinkCountDown -= elapsedTime;
            if(blinkCountDown <= 0){
                blinkCountDown = 0.5f;
            }
        }

        void Textbox::setPlaceholder(const std::string &placeholder){
            this->placeholder = placeholder;
        }
    }
}
