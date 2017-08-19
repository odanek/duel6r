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

#ifndef DUEL6_RENDERER_RENDERER_H
#define DUEL6_RENDERER_RENDERER_H

#include "../Vector.h"
#include "../Matrix.h"
#include "../Color.h"
#include "../Texture.h"
#include "../Image.h"

namespace Duel6
{
    class Material
    {
        Color color;
        Texture texture;
    };

    class Renderer
    {
    public:
        virtual void initialize() = 0;

        virtual Texture createTexture(const Image& image, TextureFilter filtering, bool clamp) = 0;
        virtual void freeTexture(Texture texture) = 0;

        virtual void readScreenData(Int32 width, Int32 height, Image& image) = 0;

        virtual void setViewport(Int32 x, Int32 y, Int32 width, Int32 height) = 0;

        virtual void setProjectionMatrix(const Matrix& m) = 0;
        virtual Matrix getProjectionMatrix() const = 0;

        virtual void setViewMatrix(const Matrix& m) = 0;
        virtual Matrix getViewMatrix() const = 0;

        virtual void setModelMatrix(const Matrix& m) = 0;
        virtual Matrix getModelMatrix() const = 0;



//        virtual void block(const Vector& position, const Vector& size, const Material& material, const Matrix& transform = Matrix::IDENTITY) = 0;
//        virtual void quad(const Vector& position, const Vector& size, const Material& material, const Matrix& transform = Matrix::IDENTITY) = 0;
//        virtual void orthoQuad(Vector position, Vector size, const Material& material, const Matrix& transform = Matrix::IDENTITY) = 0;
    };
}

#endif