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

#ifndef DUEL6_SCREENPARAMETERS_H
#define DUEL6_SCREENPARAMETERS_H

#include "Type.h"

namespace Duel6 {
    class ScreenParameters {
    private:
        Int32 clientWidth;
        Int32 clientHeight;
        Int32 bitsPerPixel;
        Int32 aaSamples;
        bool fullScreen;

    public:
        ScreenParameters() {}

        ScreenParameters(Int32 width, Int32 height, Int32 bpp, Int32 aa, bool fullScreen)
                : clientWidth(width), clientHeight(height), bitsPerPixel(bpp), aaSamples(aa), fullScreen(fullScreen) {}

        Int32 getClientWidth() const {
            return clientWidth;
        }

        Int32 getClientHeight() const {
            return clientHeight;
        }

        bool isFullScreen() const {
            return fullScreen;
        }

        Int32 getBitsPerPixel() const {
            return bitsPerPixel;
        }

        Int32 getAntiAlias() const {
            return aaSamples;
        }

        Float32 getAspect() const {
            return Float32(clientWidth) / Float32(clientHeight);
        }
    };
}

#endif