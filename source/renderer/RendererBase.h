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

#ifndef DUEL6_RENDERER_RENDERER_BASE_H
#define DUEL6_RENDERER_RENDERER_BASE_H

#include "Renderer.h"
#include "RendererTarget.h"
namespace Duel6 {

    class RendererBase
            : public Renderer {
    protected:
        Matrix projectionMatrix;
        Matrix viewMatrix;
        Matrix modelMatrix;
        Matrix mvpMatrix;

    public:
        RendererBase();

        void setProjectionMatrix(const Matrix &m) override;

        Matrix getProjectionMatrix() const override;

        void setViewMatrix(const Matrix &m) override;

        Matrix getViewMatrix() const override;

        void setModelMatrix(const Matrix &m) override;

        Matrix getModelMatrix() const override;

        void quadXY(const Vector &position, const Vector &size, const Color &color) override;

        void quadXY(const Vector &position, const Vector &size, const Vector &texturePosition,
                    const Vector &textureSize, const Material &material) override;

        void quadXZ(const Vector &position, const Vector &size, const Color &color) override;

        void quadXZ(const Vector &position, const Vector &size, const Vector &texturePosition,
                    const Vector &textureSize, const Material &material) override;

        void quadYZ(const Vector &position, const Vector &size, const Color &color) override;

        void quadYZ(const Vector &position, const Vector &size, const Vector &texturePosition,
                    const Vector &textureSize, const Material &material) override;

        void frame(const Vector &position, const Vector &size, Float32 width, const Color &color) override;
        std::unique_ptr<RendererTarget> makeTarget(GLuint width, GLuint height) override;
    };
}

#endif