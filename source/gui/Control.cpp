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

#include "Control.h"
#include "Desktop.h"
#include "../Video.h"

namespace Duel6 {
    namespace Gui {
        namespace {
            Color frameLightColor(235, 235, 235), frameDarkColor(0, 0, 0), frameFocusColor(0,50,255, 127);
        }

        Control::Control(Desktop &desk) {
            desk.addControl(this);
        }

        void Control::drawFocusFrame(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h) {
            auto l = x - 1;
            auto r = x + w + 1;
            auto b = y - h - 1;
            auto t = y - 1;
            auto z = 0.5;
            renderer.line(Vector(l, t, z), Vector(r, t, z), 4.0f, frameFocusColor);
            renderer.line(Vector(l, b, z), Vector(r, b, z), 4.0f, frameFocusColor);
            renderer.line(Vector(l, t, z), Vector(l, b, z), 4.0f, frameFocusColor);
            renderer.line(Vector(r, t, z), Vector(r, b, z), 4.0f, frameFocusColor);
        }
        void Control::drawFrame(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h, bool p, bool focus) {
            w--;
            h--;

            const Color &topColor = p ? frameDarkColor : frameLightColor;
            const Color &bottomColor = p ? frameLightColor : frameDarkColor;

            renderer.line(Vector(x, y), Vector(x, y - h), 1.0f, topColor);
            renderer.line(Vector(x + 1, y), Vector(x + 1, y - h + 1), 1.0f, topColor);
            renderer.line(Vector(x, y), Vector(x + w, y), 1.0f, topColor);
            renderer.line(Vector(x, y - 1), Vector(x + w - 1, y - 1), 1.0f, topColor);

            renderer.line(Vector(x + w, y - h), Vector(x + w, y), 1.0f, bottomColor);
            renderer.line(Vector(x + w - 1, y - h), Vector(x + w - 1, y - 1), 1.0f, bottomColor);
            renderer.line(Vector(x + w, y - h), Vector(x, y - h), 1.0f, bottomColor);
            renderer.line(Vector(x + w, y - h + 1), Vector(x + 1, y - h + 1), 1.0f, bottomColor);

            if(focus){
                drawFocusFrame(renderer,x,y,w,h);
            }
        }

        void Control::setParent(Desktop *desktop) {
            this->parent = desktop;
        }
        void Control::setFocusable(bool value) {
            this->focusable = value;
        }
    }
}
