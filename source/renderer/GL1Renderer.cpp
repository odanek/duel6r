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

#include "GL1Renderer.h"

namespace Duel6
{
    GL1Renderer::GL1Renderer(SDL_GLContext context)
        : context(context), projectionMatrix(Matrix::IDENTITY), viewMatrix(Matrix::IDENTITY), modelMatrix(Matrix::IDENTITY)
    {}

    void GL1Renderer::initialize()
    {
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glCullFace(GL_FRONT);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    Texture GL1Renderer::createTexture(const Image& image, TextureFilter filtering, bool clamp)
    {
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, (GLsizei)image.getWidth(), (GLsizei)image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &image.at(0));

        GLint filter = filtering == TextureFilter::NEAREST ? GL_NEAREST : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

        // Clamp texture coordinates
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        return Texture(textureId);
    }

    void GL1Renderer::freeTexture(Texture texture)
    {
        GLuint id = texture.getId();
        glDeleteTextures(1, &id);
    }

    void GL1Renderer::readScreenData(Int32 width, Int32 height, Image& image)
    {
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &image.at(0));
    }

    void GL1Renderer::setViewport(Int32 x, Int32 y, Int32 width, Int32 height)
    {
        glViewport(x, y, width, height);
    }

    void GL1Renderer::setProjectionMatrix(const Matrix& m)
    {
        projectionMatrix = m;
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(m.getStorage());
    }

    Matrix GL1Renderer::getProjectionMatrix() const
    {
        return projectionMatrix;
    }

    void GL1Renderer::setViewMatrix(const Matrix& m)
    {
        viewMatrix = m;
        Matrix modelView = viewMatrix * modelMatrix;
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(modelView.getStorage());
    }

    Matrix GL1Renderer::getViewMatrix() const
    {
        return viewMatrix;
    }

    void GL1Renderer::setModelMatrix(const Matrix& m)
    {
        modelMatrix = m;
        Matrix modelView = viewMatrix * modelMatrix;
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(modelView.getStorage());
    }

    Matrix GL1Renderer::getModelMatrix() const
    {
        return modelMatrix;
    }

    void GL1Renderer::enableFaceCulling(bool enable)
    {
        enableOption(GL_CULL_FACE, enable);
    }

    void GL1Renderer::enableWireframe(bool enable)
    {
        glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
    }

    void GL1Renderer::enableDepthTest(bool enable)
    {
        enableOption(GL_DEPTH_TEST, enable);
    }

    void GL1Renderer::enableDepthWrite(bool enable)
    {
        glDepthMask(GLboolean(enable ? GL_TRUE : GL_FALSE));
    }

    void GL1Renderer::clearBuffers()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GL1Renderer::enableOption(GLenum option, bool enable)
    {
        if (enable)
        {
            glEnable(option);
        }
        else
        {
            glDisable(option);
        }
    }
}