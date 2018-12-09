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

#ifndef DUEL6_RENDERER_GL4RENDERER_H
#define DUEL6_RENDERER_GL4RENDERER_H

#ifdef D6_GLEW

#include <GL/glew.h>

#else
#include <SDL2/SDL2_opengl.h>
#endif

#include "RendererBase.h"
#include "../Vertex.h"

namespace Duel6 {
    class GL4Renderer
            : public RendererBase {
    private:
        GLuint colorVao;
        GLuint colorVbo;
        GLuint colorProgram;
        GLuint materialVbo;
        GLuint materialVao;
        GLuint materialProgram;

    public:
        GL4Renderer();

        void initialize() override;

        Info getInfo() override;

        Extensions getExtensions() override;

        Texture::Id createTexture(Int32 width, Int32 height, Int32 depth, void *data, Int32 alignment,
                                  TextureFilter filtering, bool clamp) override;

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

        void quad(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, const Color &color) override;

        void quad(const Vector &p1, const Vector &t1,
                  const Vector &p2, const Vector &t2,
                  const Vector &p3, const Vector &t3,
                  const Vector &p4, const Vector &t4,
                  const Material &material) override;

        Buffer makeBuffer(const FaceList &faceList) override;

        void updateBuffer(const FaceList &faceList, const Buffer &buffer) override;

        void destroyBuffer(Buffer &buffer) override;

        void buffer(Buffer buffer, const Material &material) override;

    private:
        void enableOption(GLenum option, bool enable);

        void updateColorBuffer(Int32 vertexCount);

        void updateMaterialBuffer(Int32 vertexCount);

        void createFaceListVertexBuffer(const FaceList &faceList, std::vector<Vertex> &vertexBuffer);

        void createFaceListTextureIndexBuffer(const FaceList &faceList, std::vector<Float32> &textureIndexBuffer);
    };
}

#endif