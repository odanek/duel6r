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

#include <SDL2/SDL_keycode.h>
#include "TextBox.h"
#include "../Video.h"

namespace Duel6 {
    namespace Gui {
        Textbox::Textbox(Desktop &desk)
                : Control(desk) {
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

        void Textbox::keyEvent(const KeyPressEvent &event) {
            if (!text.empty() && event.getCode() == SDLK_BACKSPACE) {
                text.pop_back();
            }
        }

        void Textbox::textInputEvent(const TextInputEvent &event) {
            const std::string &newText = event.getText();
            for (auto iter = newText.cbegin(); iter != newText.cend(); ++iter) {
                char letter = *iter;
                if ((int) text.length() < max && allowedCharacters.find(letter) != std::string::npos) {
                    text.push_back(letter);
                }
            }
        }

        const std::string &Textbox::getText() const {
            return text;
        }

        void Textbox::flush() {
            text.clear();
        }

        void Textbox::draw(const Font &font) const {
            int w = (width << 3) + 8;

            drawFrame(x - 2, y + 2, w + 4, 22, true);
            globRenderer->quadXY(Vector(x, y - 17), Vector(w, 17), Color::WHITE);
            font.print(x, y - 16, Color(0), text + "_");
        }
    }
}
