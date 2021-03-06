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

#ifndef DUEL6_RENDERER_GLES2RENDERER_H
#define DUEL6_RENDERER_GLES2RENDERER_H

#include <GL/glew.h>
#include "../RendererBase.h"

namespace Duel6 {
    class GLES2Renderer
            : public RendererBase {
    private:
        Matrix projectionMatrix;
        Matrix viewMatrix;
        Matrix modelMatrix;
        Matrix mvpMatrix;
        GLuint colorProgram;
        GLuint textureProgram;

    public:
        GLES2Renderer();

        Info getInfo() override;

        Texture::Id createTexture(const Image &image, TextureFilter filtering, bool clamp) override;

        void freeTexture(Texture::Id textureId) override;

        void readScreenData(Int32 width, Int32 height, Image &image) override;

        void setViewport(Int32 x, Int32 y, Int32 width, Int32 height) override;

        void enableWireframe(bool enable) override;

        void enableDepthTest(bool enable) override;

        void enableDepthWrite(bool enable) override;

        void setBlendFunc(BlendFunc func) override;

        void clearBuffers() override;

        void point(const Vector &position, Float32 size, const Color &color) override;

        void line(const Vector &from, const Vector &to, Float32 width, const Color &color) override;

        void triangle(const Vector &p1, const Vector &p2, const Vector &p3, const Color &color) override;

        void triangle(const Vector &p1, const Vector &t1,
                      const Vector &p2, const Vector &t2,
                      const Vector &p3, const Vector &t3,
                      const Material &material) override;

    private:
        void enableOption(GLenum option, bool enable);
    };
}

#endif