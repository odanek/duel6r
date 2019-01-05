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

#include "GLES2Renderer.h"

namespace Duel6 {
    static float points[15];

    static const char *colorVertexShader =
            "attribute vec3 vp;"
                    "uniform mat4 mvp;"
                    "uniform float pointSize;"
                    "void main() {"
                    "  gl_PointSize = pointSize;"
                    "  gl_Position = mvp * vec4(vp, 1.0);"
                    "}";
    static const char *textureVertexShader =
            "uniform mat4 mvp;"
                    "attribute vec3 vp;"
                    "attribute vec2 uvIn;"
                    "varying vec2 uvFrag;"
                    "void main() {"
                    "  gl_Position = mvp * vec4(vp, 1.0);"
                    "  uvFrag = uvIn;"
                    "}";


    static const char *colorFragmentShader =
            "precision highp float;"
                    "uniform vec4 color;"
                    "void main() {"
                    "  gl_FragColor = color;"
                    "}";
    static const char *textureFragmentShader =
            "precision highp float;"
                    "varying vec2 uvFrag;"
                    "uniform sampler2D textureUnit;"
                    "uniform bool alphaTest;"
                    "uniform vec4 modulateColor;"
                    "void main() {"
                    "  vec4 color = texture2D(textureUnit, uvFrag);"
                    "  if (alphaTest && color.w < 1.0) discard;"
                    "  gl_FragColor = color * modulateColor;"
                    "}";


    GLES2Renderer::GLES2Renderer()
            : RendererBase() {
        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        enableOption(GL_CULL_FACE, true);

        GLuint cvs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(cvs, 1, &colorVertexShader, nullptr);
        glCompileShader(cvs);
        GLuint cfs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(cfs, 1, &colorFragmentShader, nullptr);
        glCompileShader(cfs);

        colorProgram = glCreateProgram();
        glAttachShader(colorProgram, cfs);
        glAttachShader(colorProgram, cvs);
        glBindAttribLocation(colorProgram, 0, "vp");
        glLinkProgram(colorProgram);

        GLuint tvs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(tvs, 1, &textureVertexShader, nullptr);
        glCompileShader(tvs);
        GLuint tfs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(tfs, 1, &textureFragmentShader, nullptr);
        glCompileShader(tfs);

        textureProgram = glCreateProgram();
        glAttachShader(textureProgram, tfs);
        glAttachShader(textureProgram, tvs);
        glBindAttribLocation(textureProgram, 0, "vp");
        glBindAttribLocation(textureProgram, 1, "uvIn");
        glLinkProgram(textureProgram);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, points);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, &points[9]);
    }

    Renderer::Info GLES2Renderer::getInfo() {
        Info info;
        info.vendor = (const char *) glGetString(GL_VENDOR);
        info.renderer = (const char *) glGetString(GL_RENDERER);
        info.version = (const char *) glGetString(GL_VERSION);

        const char *extensions = (const char *) glGetString(GL_EXTENSIONS);

        if (extensions != nullptr) {
            while (*extensions != 0) {
                while (*extensions == ' ') {
                    extensions++;
                }

                if (*extensions == 0) {
                    break;
                }

                std::string extensionName;
                while (*extensions != ' ' && *extensions != 0) {
                    extensionName += *extensions;
                    ++extensions;
                }

                info.extensions.push_back(extensionName);
            }
        }

        return info;
    }

    Texture::Id
    GLES2Renderer::createTexture(const Image &image, TextureFilter filtering, bool clamp) {
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        GLint filter = filtering == TextureFilter::NEAREST ? GL_NEAREST : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

        // Clamp texture coordinates
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        return textureId;
    }

    void GLES2Renderer::freeTexture(Texture::Id textureId) {
        GLuint id = textureId;
        glDeleteTextures(1, &id);
    }

    void GLES2Renderer::readScreenData(Int32 width, Int32 height, Image &image) {
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &image.at(0));
    }

    void GLES2Renderer::setViewport(Int32 x, Int32 y, Int32 width, Int32 height) {
        glViewport(x, y, width, height);
    }

    void GLES2Renderer::enableWireframe(bool enable) {
    }

    void GLES2Renderer::enableDepthTest(bool enable) {
        enableOption(GL_DEPTH_TEST, enable);
    }

    void GLES2Renderer::enableDepthWrite(bool enable) {
        glDepthMask(GLboolean(enable ? GL_TRUE : GL_FALSE));
    }

    void GLES2Renderer::setBlendFunc(BlendFunc func) {
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

    void GLES2Renderer::clearBuffers() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GLES2Renderer::point(const Vector &position, Float32 size, const Color &color) {
        glUseProgram(colorProgram);
        glEnableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        points[0] = position.x;
        points[1] = position.y;
        points[2] = position.z;

        glUniformMatrix4fv(glGetUniformLocation(colorProgram, "mvp"), 1, GL_FALSE, mvpMatrix.getStorage());
        glUniform1f(glGetUniformLocation(colorProgram, "pointSize"), size);

        Float32 colorData[] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                               color.getAlpha() / 255.0f};
        GLint location = glGetUniformLocation(colorProgram, "color");
        glUniform4fv(location, 1, colorData);

        glDrawArrays(GL_POINTS, 0, 1);
    }

    void GLES2Renderer::line(const Vector &from, const Vector &to, Float32 width, const Color &color) {
        glLineWidth(width);

        glUseProgram(colorProgram);
        glEnableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        points[0] = from.x;
        points[1] = from.y;
        points[2] = from.z;
        points[3] = to.x;
        points[4] = to.y;
        points[5] = to.z;

        glUniformMatrix4fv(glGetUniformLocation(colorProgram, "mvp"), 1, GL_FALSE, mvpMatrix.getStorage());

        Float32 colorData[] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                               color.getAlpha() / 255.0f};
        GLint location = glGetUniformLocation(colorProgram, "color");
        glUniform4fv(location, 1, colorData);

        glDrawArrays(GL_LINES, 0, 2);
        glLineWidth(1.0f);
    }

    void GLES2Renderer::triangle(const Vector &p1, const Vector &p2, const Vector &p3, const Color &color) {
        glUseProgram(colorProgram);
        glEnableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        points[0] = p1.x;
        points[1] = p1.y;
        points[2] = p1.z;
        points[3] = p2.x;
        points[4] = p2.y;
        points[5] = p2.z;
        points[6] = p3.x;
        points[7] = p3.y;
        points[8] = p3.z;

        glUniformMatrix4fv(glGetUniformLocation(colorProgram, "mvp"), 1, GL_FALSE, mvpMatrix.getStorage());
        glUniform1f(glGetUniformLocation(colorProgram, "pointSize"), 1.0f);

        Float32 colorData[] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                               color.getAlpha() / 255.0f};
        GLint location = glGetUniformLocation(colorProgram, "color");
        glUniform4fv(location, 1, colorData);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void GLES2Renderer::triangle(const Vector &p1, const Vector &t1,
                                 const Vector &p2, const Vector &t2,
                                 const Vector &p3, const Vector &t3,
                                 const Material &material) {
        glUseProgram(textureProgram);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        points[0] = p1.x;
        points[1] = p1.y;
        points[2] = p1.z;
        points[3] = p2.x;
        points[4] = p2.y;
        points[5] = p2.z;
        points[6] = p3.x;
        points[7] = p3.y;
        points[8] = p3.z;

        points[9] = t1.x;
        points[10] = t1.y;
        points[11] = t2.x;
        points[12] = t2.y;
        points[13] = t3.x;
        points[14] = t3.y;

        glUniformMatrix4fv(glGetUniformLocation(textureProgram, "mvp"), 1, GL_FALSE, mvpMatrix.getStorage());

        glBindTexture(GL_TEXTURE_2D, material.getTexture().getId());
        glUniform1i(glGetUniformLocation(textureProgram, "textureUnit"), 0);

        glUniform1i(glGetUniformLocation(textureProgram, "alphaTest"), material.isMasked() ? 1 : 0);

        const Color color = material.getColor();
        Float32 colorData[] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                               color.getAlpha() / 255.0f};
        glUniform4fv(glGetUniformLocation(textureProgram, "modulateColor"), 1, colorData);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void GLES2Renderer::enableOption(GLenum option, bool enable) {
        if (enable) {
            glEnable(option);
        } else {
            glDisable(option);
        }
    }
}
