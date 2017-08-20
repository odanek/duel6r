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

        glFrontFace(GL_CW);
        glCullFace(GL_BACK);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    Renderer::Info GL1Renderer::getInfo()
    {
        Info info;
        info.vendor = (const char *)glGetString(GL_VENDOR);
        info.renderer = (const char *)glGetString(GL_RENDERER);
        info.version = (const char *)glGetString(GL_VERSION);
        info.extensions = (const char *)glGetString(GL_EXTENSIONS);
        return info;
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

    void GL1Renderer::setBlendFunc(Renderer::BlendFunc func)
    {
        switch (func)
        {
            case BlendFunc::None:
                glDisable(GL_BLEND);
                break;
            case BlendFunc::SrcAlpha:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case BlendFunc::SrcColor:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
                break;
        }
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

    void GL1Renderer::triangle(const Vector& p1, const Vector& p2, const Vector& p3, const Color& color)
    {
        glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());

        glBegin(GL_TRIANGLES);
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glEnd();

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }

    void GL1Renderer::triangle(const Vector& p1, const Vector& t1,
                               const Vector& p2, const Vector& t2,
                               const Vector& p3, const Vector& t3,
                               const Texture& texture)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture.getId());

        glBegin(GL_TRIANGLES);
        glTexCoord2f(t1.x, t1.y);
        glVertex3f(p1.x, p1.y, p1.z);
        glTexCoord2f(t2.x, t2.y);
        glVertex3f(p2.x, p2.y, p2.z);
        glTexCoord2f(t3.x, t3.y);
        glVertex3f(p3.x, p3.y, p3.z);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    void GL1Renderer::quadXY(const Vector &position, const Vector &size, const Color &color)
    {
        Vector p2 = Vector(position.x, position.y + size.y, position.z);
        Vector p3 = Vector(position.x + size.x, position.y + size.y, position.z);
        Vector p4 = Vector(position.x + size.x, position.y, position.z);

        triangle(position, p2, p3, color);
        triangle(position, p3, p4, color);
    }

    void GL1Renderer::quadXY(const Vector &position, const Vector &size,
                           const Vector &texturePosition, const Vector &textureSize,
                           const Texture &texture)
    {
        Vector p2 = Vector(position.x, position.y + size.y, position.z);
        Vector t2 = Vector(texturePosition.x, texturePosition.y + textureSize.y);
        Vector p3 = Vector(position.x + size.x, position.y + size.y, position.z);
        Vector t3 = Vector(texturePosition.x + textureSize.x, texturePosition.y + textureSize.y);
        Vector p4 = Vector(position.x + size.x, position.y, position.z);
        Vector t4 = Vector(texturePosition.x + textureSize.x, texturePosition.y);

        triangle(position, texturePosition, p2, t2, p3, t3, texture);
        triangle(position, texturePosition, p3, t3, p4, t4, texture);
    }

    void GL1Renderer::quadXZ(const Vector &position, const Vector &size, const Color &color)
    {
        Vector p2 = Vector(position.x + size.x, position.y, position.z);
        Vector p3 = Vector(position.x + size.x, position.y, position.z + size.z);
        Vector p4 = Vector(position.x, position.y, position.z + size.z);

        triangle(position, p2, p3, color);
        triangle(position, p3, p4, color);
    }

    void GL1Renderer::quadXZ(const Vector &position, const Vector &size,
                             const Vector &texturePosition, const Vector &textureSize,
                             const Texture &texture)
    {
        Vector p2 = Vector(position.x + size.x, position.y, position.z);
        Vector t2 = Vector(texturePosition.x + textureSize.x, texturePosition.y);
        Vector p3 = Vector(position.x + size.x, position.y, position.z + size.z);
        Vector t3 = Vector(texturePosition.x + textureSize.x, texturePosition.y + textureSize.y);
        Vector p4 = Vector(position.x, position.y, position.z + size.z);
        Vector t4 = Vector(texturePosition.x, texturePosition.y + textureSize.y);

        triangle(position, texturePosition, p2, t2, p3, t3, texture);
        triangle(position, texturePosition, p3, t3, p4, t4, texture);
    }

    void GL1Renderer::quadYZ(const Vector &position, const Vector &size, const Color &color)
    {
        Vector p2 = Vector(position.x, position.y + size.y, position.z);
        Vector p3 = Vector(position.x, position.y + size.y, position.z + size.z);
        Vector p4 = Vector(position.x, position.y, position.z + size.z);

        triangle(position, p2, p3, color);
        triangle(position, p3, p4, color);
    }

    void GL1Renderer::quadYZ(const Vector &position, const Vector &size,
                             const Vector &texturePosition, const Vector &textureSize,
                             const Texture &texture)
    {
        Vector p2 = Vector(position.x, position.y + size.y, position.z);
        Vector t2 = Vector(texturePosition.x, texturePosition.y + textureSize.y);
        Vector p3 = Vector(position.x, position.y + size.y, position.z + size.z);
        Vector t3 = Vector(texturePosition.x + textureSize.x, texturePosition.y + textureSize.y);
        Vector p4 = Vector(position.x, position.y, position.z + size.z);
        Vector t4 = Vector(texturePosition.x + textureSize.x, texturePosition.y);

        triangle(position, texturePosition, p2, t2, p3, t3, texture);
        triangle(position, texturePosition, p3, t3, p4, t4, texture);
    }
}