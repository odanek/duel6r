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

#include "GL4Renderer.h"

namespace Duel6 {
    static float points[15];

    static const char *colorVertexShader =
            "#version 430\n"
                    "uniform mat4 mvp;"
                    "in vec3 vp;"
                    "void main() {"
                    "  gl_Position = mvp * vec4(vp, 1.0);"
                    "}";
    static const char *textureVertexShader =
            "#version 430\n"
                    "uniform mat4 mvp;"
                    "layout(location = 0) in vec3 vp;"
                    "layout(location = 1) in vec2 uvIn;"
                    "out vec2 uv;"
                    "void main() {"
                    "  gl_Position = mvp * vec4(vp, 1.0);"
                    "  uv = uvIn;"
                    "}";


    static const char *colorFragmentShader =
            "#version 430\n"
                    "uniform vec4 color;"
                    "out vec4 result;"
                    "void main() {"
                    "  result = color;"
                    "}";
    static const char *textureFragmentShader =
            "#version 430\n"
                    "in vec2 uv;"
                    "out vec4 result;"
                    "uniform sampler2D textureUnit;"
                    "void main() {"
                    "  result = texture2D(textureUnit, uv);"
                    "}";

    GL4Renderer::GL4Renderer()
            : projectionMatrix(Matrix::IDENTITY), viewMatrix(Matrix::IDENTITY), modelMatrix(Matrix::IDENTITY) {}

    void GL4Renderer::initialize() {
#ifdef D6_GLEW
        glewInit();
#endif

        glFrontFace(GL_CW);
        glCullFace(GL_BACK);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(float), points, GL_DYNAMIC_DRAW);

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *) (9 * sizeof(Float32)));

        GLuint cvs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(cvs, 1, &colorVertexShader, nullptr);
        glCompileShader(cvs);
        GLuint cfs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(cfs, 1, &colorFragmentShader, nullptr);
        glCompileShader(cfs);

        colorTriangleProgram = glCreateProgram();
        glAttachShader(colorTriangleProgram, cfs);
        glAttachShader(colorTriangleProgram, cvs);
        glLinkProgram(colorTriangleProgram);

        GLuint tvs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(tvs, 1, &textureVertexShader, nullptr);
        glCompileShader(tvs);
        GLuint tfs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(tfs, 1, &textureFragmentShader, nullptr);
        glCompileShader(tfs);

        textureTriangleProgram = glCreateProgram();
        glAttachShader(textureTriangleProgram, tfs);
        glAttachShader(textureTriangleProgram, tvs);
        glLinkProgram(textureTriangleProgram);

        glActiveTexture(GL_TEXTURE0);
    }

    Renderer::Info GL4Renderer::getInfo() {
        Info info;
        info.vendor = (const char *) glGetString(GL_VENDOR);
        info.renderer = (const char *) glGetString(GL_RENDERER);
        info.version = (const char *) glGetString(GL_VERSION);

        GLint numExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

        for (Int32 i = 0; i < numExtensions; i++) {
            std::string name = (const char *) glGetStringi(GL_EXTENSIONS, i);
            info.extensions.push_back(name);
        }

        return info;
    }

    Texture GL4Renderer::createTexture(Int32 width, Int32 height, void *data, Int32 alignment, TextureFilter filtering,
                                       bool clamp) {
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        GLint filter = filtering == TextureFilter::NEAREST ? GL_NEAREST : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

        // Clamp texture coordinates
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        return Texture(textureId);
    }

    void GL4Renderer::setTextureFilter(const Texture &texture, TextureFilter filter) {
        glBindTexture(GL_TEXTURE_2D, texture.getId());
        GLint filterValue = filter == TextureFilter::NEAREST ? GL_NEAREST : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterValue);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterValue);
    }

    void GL4Renderer::freeTexture(Texture texture) {
        GLuint id = texture.getId();
        glDeleteTextures(1, &id);
    }

    void GL4Renderer::readScreenData(Int32 width, Int32 height, Image &image) {
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &image.at(0));
    }

    void GL4Renderer::setViewport(Int32 x, Int32 y, Int32 width, Int32 height) {
        glViewport(x, y, width, height);
    }

    void GL4Renderer::setProjectionMatrix(const Matrix &m) {
        projectionMatrix = m;
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    }

    Matrix GL4Renderer::getProjectionMatrix() const {
        return projectionMatrix;
    }

    void GL4Renderer::setViewMatrix(const Matrix &m) {
        viewMatrix = m;
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    }

    Matrix GL4Renderer::getViewMatrix() const {
        return viewMatrix;
    }

    void GL4Renderer::setModelMatrix(const Matrix &m) {
        modelMatrix = m;
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    }

    Matrix GL4Renderer::getModelMatrix() const {
        return modelMatrix;
    }

    void GL4Renderer::enableFaceCulling(bool enable) {
        enableOption(GL_CULL_FACE, enable);
    }

    void GL4Renderer::enableWireframe(bool enable) {
    }

    void GL4Renderer::enableDepthTest(bool enable) {
        enableOption(GL_DEPTH_TEST, enable);
    }

    void GL4Renderer::enableDepthWrite(bool enable) {
        glDepthMask(GLboolean(enable ? GL_TRUE : GL_FALSE));
    }

    void GL4Renderer::setBlendFunc(Renderer::BlendFunc func) {
        switch (func) {
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

    void GL4Renderer::clearBuffers() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GL4Renderer::triangle(const Vector &p1, const Vector &p2, const Vector &p3, const Color &color) {
        glUseProgram(colorTriangleProgram);

        glUniformMatrix4fv(glGetUniformLocation(colorTriangleProgram, "mvp"), 1, GL_FALSE, mvpMatrix.getStorage());

        Float32 points[] = {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z};
        glBufferSubData(GL_ARRAY_BUFFER, 0, 9 * sizeof(Float32), points);

        Float32 colorData[] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                               color.getAlpha() / 255.0f};
        GLint location = glGetUniformLocation(colorTriangleProgram, "color");
        glUniform4fv(location, 1, colorData);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void GL4Renderer::triangle(const Vector &p1, const Vector &t1,
                               const Vector &p2, const Vector &t2,
                               const Vector &p3, const Vector &t3,
                               const Material &material) {
        glUseProgram(textureTriangleProgram);

        glBindTexture(GL_TEXTURE_2D, material.getTexture().getId());
        glUniform1i(glGetUniformLocation(textureTriangleProgram, "textureUnit"), 0);

        glUniformMatrix4fv(glGetUniformLocation(textureTriangleProgram, "mvp"), 1, GL_FALSE, mvpMatrix.getStorage());

        Float32 points[] = {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z, t1.x, t1.y, t2.x, t2.y, t3.x, t3.y};
        glBufferSubData(GL_ARRAY_BUFFER, 0, 15 * sizeof(Float32), points);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void GL4Renderer::quadXY(const Vector &position, const Vector &size, const Color &color) {
        Vector p2(position.x, position.y + size.y, position.z);
        Vector p3(position.x + size.x, position.y + size.y, position.z);
        Vector p4(position.x + size.x, position.y, position.z);

        triangle(position, p2, p3, color);
        triangle(position, p3, p4, color);
    }

    void GL4Renderer::quadXY(const Vector &position, const Vector &size,
                             const Vector &texturePosition, const Vector &textureSize,
                             const Material &material) {
        Vector p2(position.x, position.y + size.y, position.z);
        Vector t2(texturePosition.x, texturePosition.y + textureSize.y);
        Vector p3(position.x + size.x, position.y + size.y, position.z);
        Vector t3(texturePosition.x + textureSize.x, texturePosition.y + textureSize.y);
        Vector p4(position.x + size.x, position.y, position.z);
        Vector t4(texturePosition.x + textureSize.x, texturePosition.y);

        triangle(position, texturePosition, p2, t2, p3, t3, material);
        triangle(position, texturePosition, p3, t3, p4, t4, material);
    }

    void GL4Renderer::quadXZ(const Vector &position, const Vector &size, const Color &color) {
        Vector p2(position.x + size.x, position.y, position.z);
        Vector p3(position.x + size.x, position.y, position.z + size.z);
        Vector p4(position.x, position.y, position.z + size.z);

        triangle(position, p2, p3, color);
        triangle(position, p3, p4, color);
    }

    void GL4Renderer::quadXZ(const Vector &position, const Vector &size,
                             const Vector &texturePosition, const Vector &textureSize,
                             const Material &material) {
        Vector p2(position.x + size.x, position.y, position.z);
        Vector t2(texturePosition.x + textureSize.x, texturePosition.y);
        Vector p3(position.x + size.x, position.y, position.z + size.z);
        Vector t3(texturePosition.x + textureSize.x, texturePosition.y + textureSize.y);
        Vector p4(position.x, position.y, position.z + size.z);
        Vector t4(texturePosition.x, texturePosition.y + textureSize.y);

        triangle(position, texturePosition, p2, t2, p3, t3, material);
        triangle(position, texturePosition, p3, t3, p4, t4, material);
    }

    void GL4Renderer::quadYZ(const Vector &position, const Vector &size, const Color &color) {
        Vector p2(position.x, position.y + size.y, position.z);
        Vector p3(position.x, position.y + size.y, position.z + size.z);
        Vector p4(position.x, position.y, position.z + size.z);

        triangle(position, p2, p3, color);
        triangle(position, p3, p4, color);
    }

    void GL4Renderer::quadYZ(const Vector &position, const Vector &size,
                             const Vector &texturePosition, const Vector &textureSize,
                             const Material &material) {
        Vector p2(position.x, position.y + size.y, position.z);
        Vector t2(texturePosition.x, texturePosition.y + textureSize.y);
        Vector p3(position.x, position.y + size.y, position.z + size.z);
        Vector t3(texturePosition.x + textureSize.x, texturePosition.y + textureSize.y);
        Vector p4(position.x, position.y, position.z + size.z);
        Vector t4(texturePosition.x + textureSize.x, texturePosition.y);

        triangle(position, texturePosition, p2, t2, p3, t3, material);
        triangle(position, texturePosition, p3, t3, p4, t4, material);
    }

    void GL4Renderer::point(const Vector &position, Float32 size, const Color &color) {
    }

    void GL4Renderer::line(const Vector &from, const Vector &to, Float32 width, const Color &color) {
    }

    void GL4Renderer::frame(const Vector &position, const Vector &size, Float32 width, const Color &color) {
        Vector p2(position.x, position.y + size.y);
        Vector p3(position.x + size.x, position.y + size.y);
        Vector p4(position.x + size.x, position.y);

        line(position, p2, width, color);
        line(p2, p3, width, color);
        line(p3, p4, width, color);
        line(p4, position, width, color);
    }

    void GL4Renderer::enableOption(GLenum option, bool enable) {
        if (enable) {
            glEnable(option);
        } else {
            glDisable(option);
        }
    }
}