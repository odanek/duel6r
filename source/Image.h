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

#ifndef DUEL6_IMAGE_H
#define DUEL6_IMAGE_H

#include <SDL2/SDL.h>
#include <vector>
#include "Type.h"
#include "Color.h"

namespace Duel6 {
    class Image {
    private:
        Size dimensions[3];
        std::vector<Color> data;

    public:
        Image() : Image(0, 0, (Size)0) {}

        Image(Size width, Size height, Size depth = 1);

        Image &resize(Size width, Size height, Size depth = 1);

        Size getWidth() const {
            return dimensions[0];
        }

        Size getHeight() const {
            return dimensions[1];
        }

        Size getDepth() const {
            return dimensions[2];
        }

        Color &at(Size index) {
            return data[index];
        }

        const Color &at(Size index) const {
            return data[index];
        }

        Image flipY() const;

        Image &addSlice(const Image &slice);

        void save(const std::string &path) const;

        std::string saveScreenshot() const;

        static Image load(const std::string &path);

        static Image loadStack(const std::string &path);

        static Image fromSurface(SDL_Surface *surface);
    };
}

#endif
