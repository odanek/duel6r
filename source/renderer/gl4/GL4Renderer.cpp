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
#include "../../FaceList.h"
#include "../../Exception.h"
#include "../../VideoException.h"
#include "../../File.h"

namespace Duel6 {
    struct ColorVertex {
        Vector xyz;
    };
    static ColorVertex colorPoints[4];

    struct MaterialVertex {
        Vector xyz; // Position
        Vector str; // Texture coordinates
        Uint32 flags;
    };
    static MaterialVertex materialPoints[4];

    GL4Renderer::GL4Renderer()
            : RendererBase(),
              colorVertexShader(GL_VERTEX_SHADER, "shaders/gl4/colorVertex.glsl"),
              colorFragmentShader(GL_FRAGMENT_SHADER, "shaders/gl4/colorFragment.glsl"),
              materialVertexShader(GL_VERTEX_SHADER, "shaders/gl4/materialVertex.glsl"),
              materialFragmentShader(GL_FRAGMENT_SHADER, "shaders/gl4/materialFragment.glsl"),
              colorProgram(colorVertexShader, colorFragmentShader),
              materialProgram(materialVertexShader, materialFragmentShader) {
        memset(materialPoints, 0, sizeof(MaterialVertex) * 4);

        enableOption(GL_CULL_FACE, true);
        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glActiveTexture(GL_TEXTURE0);

        glGenVertexArrays(1, &colorVao);
        glBindVertexArray(colorVao);

        glGenBuffers(1, &colorVbo);
        glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(ColorVertex), colorPoints, GL_STREAM_DRAW);
        //glNamedBufferStorage(colorVbo, 4 * sizeof(ColorVertex), colorPoints, GL_DYNAMIC_STORAGE_BIT);  // GL 4.5

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glGenVertexArrays(1, &materialVao);
        glBindVertexArray(materialVao);

        glGenBuffers(1, &materialVbo);
        glBindBuffer(GL_ARRAY_BUFFER, materialVbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(MaterialVertex), materialPoints, GL_STREAM_DRAW);
        //glNamedBufferStorage(materialVbo, 4 * sizeof(MaterialVertex), materialPoints, GL_DYNAMIC_STORAGE_BIT);  // GL 4.5

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MaterialVertex), nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MaterialVertex), (const GLvoid *) (3 * sizeof(Float32)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(MaterialVertex), (const GLvoid *) (5 * sizeof(Float32)));
        glEnableVertexAttribArray(2);
        glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(MaterialVertex), (const GLvoid *) (6 * sizeof(Float32)));
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    Renderer::Info GL4Renderer::getInfo() {
        Info info;
        info.vendor = (const char *) glGetString(GL_VENDOR);
        info.renderer = (const char *) glGetString(GL_RENDERER);
        info.version = (const char *) glGetString(GL_VERSION);
        return info;
    }

    Renderer::Extensions GL4Renderer::getExtensions() {
        Extensions info;

        GLint numExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

        for (Int32 i = 0; i < numExtensions; i++) {
            std::string name = (const char *) glGetStringi(GL_EXTENSIONS, i);
            info.extensions.push_back(name);
        }

        return info;
    }

    Texture GL4Renderer::createTexture(const Image &image, TextureFilter filtering, bool clamp) {
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, image.getWidth(), image.getHeight(), image.getDepth(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &image.at(0));

        GLint filter = filtering == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        return textureId;
    }

    void GL4Renderer::freeTexture(Texture textureId) {
        GLuint id = textureId;
        glDeleteTextures(1, &id);
    }

    Image GL4Renderer::makeScreenshot() {
        GLint dimensions[4];
        glGetIntegerv(GL_VIEWPORT, dimensions);

        auto width = dimensions[2];
        auto height = dimensions[3];

        Image image(width, height);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &image.at(0));

        return image.flipY();
    }

    void GL4Renderer::setViewport(Int32 x, Int32 y, Int32 width, Int32 height) {
        glViewport(x, y, width, height);
    }

    void GL4Renderer::enableWireframe(bool enable) {
        glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
    }

    void GL4Renderer::enableDepthTest(bool enable) {
        enableOption(GL_DEPTH_TEST, enable);
    }

    void GL4Renderer::enableDepthWrite(bool enable) {
        glDepthMask(GLboolean(enable ? GL_TRUE : GL_FALSE));
    }

    void GL4Renderer::setBlendFunc(BlendFunc func) {
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

    void GL4Renderer::setGlobalTime(Float32 time) {
        materialProgram.bind(); // Required for INTEL
        materialProgram.setUniform("globalTime", time);
    }

    void GL4Renderer::clearBuffers() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GL4Renderer::setProjectionMatrix(const Matrix &m) {
        RendererBase::setProjectionMatrix(m);
        updateMvpUniform();
    }

    void GL4Renderer::setViewMatrix(const Matrix &m) {
        RendererBase::setViewMatrix(m);
        updateMvpUniform();
    }

    void GL4Renderer::setModelMatrix(const Matrix &m) {
        RendererBase::setModelMatrix(m);
        updateMvpUniform();
    }

    void GL4Renderer::point(const Vector &position, Float32 size, const Color &color) {
        glBindVertexArray(colorVao);
        colorProgram.bind();

        colorPoints[0].xyz = position;
        updateColorBuffer(1);

        Float32 colorData[4] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                                color.getAlpha() / 255.0f};
        colorProgram.setUniform("color", colorData);

        glPointSize(size);
        glDrawArrays(GL_POINTS, 0, 1);
        glPointSize(1);
    }

    void GL4Renderer::line(const Vector &from, const Vector &to, Float32 width, const Color &color) {
        glBindVertexArray(colorVao);
        colorProgram.bind();

        colorPoints[0].xyz = from;
        colorPoints[1].xyz = to;
        updateColorBuffer(2);

        Float32 colorData[4] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                                color.getAlpha() / 255.0f};
        colorProgram.setUniform("color", colorData);

        glLineWidth(width);
        glDrawArrays(GL_LINES, 0, 2);
        glLineWidth(1.0f);
    }

    void GL4Renderer::triangle(const Vector &p1, const Vector &p2, const Vector &p3, const Color &color) {
        glBindVertexArray(colorVao);
        colorProgram.bind();

        colorPoints[0].xyz = p1;
        colorPoints[1].xyz = p2;
        colorPoints[2].xyz = p3;
        updateColorBuffer(3);

        Float32 colorData[4] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                                color.getAlpha() / 255.0f};
        colorProgram.setUniform("color", colorData);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void GL4Renderer::triangle(const Vector &p1, const Vector &t1,
                               const Vector &p2, const Vector &t2,
                               const Vector &p3, const Vector &t3,
                               const Material &material) {
        glBindVertexArray(materialVao);
        materialProgram.bind();

        glBindTexture(GL_TEXTURE_2D_ARRAY, material.getTexture());

        materialPoints[0].xyz = p1;
        materialPoints[0].str = t1;
        materialPoints[1].xyz = p2;
        materialPoints[1].str = t2;
        materialPoints[2].xyz = p3;
        materialPoints[2].str = t3;
        updateMaterialBuffer(3);

        materialProgram.setUniform("alphaTest", material.isMasked() ? 1 : 0);

        const Color &color = material.getColor();
        Float32 colorData[4] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                                color.getAlpha() / 255.0f};
        materialProgram.setUniform("modulateColor", colorData);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void GL4Renderer::quad(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, const Color &color) {
        glBindVertexArray(colorVao);
        colorProgram.bind();

        colorPoints[0].xyz = p1;
        colorPoints[1].xyz = p2;
        colorPoints[2].xyz = p3;
        colorPoints[3].xyz = p4;
        updateColorBuffer(4);

        Float32 colorData[4] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                                color.getAlpha() / 255.0f};
        colorProgram.setUniform("color", colorData);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    void GL4Renderer::quad(const Vector &p1, const Vector &t1,
                           const Vector &p2, const Vector &t2,
                           const Vector &p3, const Vector &t3,
                           const Vector &p4, const Vector &t4,
                           const Material &material) {
        glBindVertexArray(materialVao);
        materialProgram.bind();

        glBindTexture(GL_TEXTURE_2D_ARRAY, material.getTexture());

        materialPoints[0].xyz = p1;
        materialPoints[0].str = t1;
        materialPoints[1].xyz = p2;
        materialPoints[1].str = t2;
        materialPoints[2].xyz = p3;
        materialPoints[2].str = t3;
        materialPoints[3].xyz = p4;
        materialPoints[3].str = t4;
        updateMaterialBuffer(4);

        materialProgram.setUniform("alphaTest", material.isMasked() ? 1 : 0);

        const Color &color = material.getColor();
        Float32 colorData[4] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                                color.getAlpha() / 255.0f};
        materialProgram.setUniform("modulateColor", colorData);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    std::unique_ptr<RendererBuffer> GL4Renderer::makeBuffer(const FaceList &faceList) {
        return std::make_unique<GL4Buffer>(materialProgram, faceList);
    }

    void GL4Renderer::enableOption(GLenum option, bool enable) {
        if (enable) {
            glEnable(option);
        } else {
            glDisable(option);
        }
    }

    void GL4Renderer::updateColorBuffer(Int32 vertexCount) {
        glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(ColorVertex), colorPoints, GL_STREAM_DRAW);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(ColorVertex), colorPoints);
        // glNamedBufferSubData(triangleVbo, 0, vertexCount * sizeof(VertexData), points); // GL 4.5
    }

    void GL4Renderer::updateMaterialBuffer(Int32 vertexCount) {
        glBindBuffer(GL_ARRAY_BUFFER, materialVbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(MaterialVertex), materialPoints, GL_STREAM_DRAW);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(MaterialVertex), materialPoints);
        // glNamedBufferSubData(triangleVbo, 0, vertexCount * sizeof(VertexData), points); // GL 4.5
    }

    void GL4Renderer::updateMvpUniform() {
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

        colorProgram.bind(); // Required for INTEL
        colorProgram.setUniform("mvp", mvpMatrix);
        materialProgram.bind(); // Required for INTEL
        materialProgram.setUniform("mvp", mvpMatrix);
    }
}
