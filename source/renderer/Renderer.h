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

#include <memory>
#include <vector>
#include "../math/Vector.h"
#include "../math/Matrix.h"
#include "../Color.h"
#include "../Image.h"
#include "../Material.h"
#include "../ScreenParameters.h"
#include "RendererTypes.h"
#include "RendererBuffer.h"
#include "RendererTarget.h"

namespace Duel6 {
    class FaceList;

    class RendererTarget;

    class Renderer {
    public:
        enum class StencilMode {
            DISABLE,
            CLEAR_AND_WRITE,
            TEST
        };

        struct Info {
            std::string vendor;
            std::string renderer;
            std::string version;
        };

        struct Extensions {
            std::vector<std::string> extensions;
        };

    public:
        virtual ~Renderer() = default;

        virtual Info getInfo() = 0;

        virtual Extensions getExtensions() = 0;

        virtual Texture createTexture(const Image &image, TextureFilter filtering, bool clamp) = 0;

        virtual void freeTexture(Texture textureId) = 0;

        virtual Image makeScreenshot() = 0;

        virtual void setViewport(Int32 x, Int32 y, Int32 width, Int32 height) = 0;

        virtual void setProjectionMatrix(const Matrix &m) = 0;

        virtual Matrix getProjectionMatrix() const = 0;

        virtual void setViewMatrix(const Matrix &m) = 0;

        virtual Matrix getViewMatrix() const = 0;

        virtual void setModelMatrix(const Matrix &m) = 0;

        virtual Matrix getModelMatrix() const = 0;

        virtual void enableWireframe(bool enable) = 0;

        virtual void enableDepthTest(bool enable) = 0;

        virtual void enableDepthWrite(bool enable) = 0;

        virtual void setBlendFunc(BlendFunc func) = 0;

        virtual void setGlobalTime(Float32 time) = 0;

        virtual void clearBuffers() = 0;

        virtual void triangle(const Vector &p1, const Vector &p2, const Vector &p3, const Color &color) = 0;

        virtual void triangle(const Vector &p1, const Vector &t1,
                              const Vector &p2, const Vector &t2,
                              const Vector &p3, const Vector &t3,
                              const Material &material) = 0;

        virtual void
        quad(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, const Color &color) = 0;

        virtual void quad(const Vector &p1, const Vector &t1,
                          const Vector &p2, const Vector &t2,
                          const Vector &p3, const Vector &t3,
                          const Vector &p4, const Vector &t4,
                          const Material &material) = 0;

        virtual void quadXY(const Vector &position, const Vector &size, const Color &color) = 0;

        virtual void quadXY(const Vector &position, const Vector &size, const Vector &texturePosition,
                            const Vector &textureSize, const Material &material) = 0;

        virtual void quadXZ(const Vector &position, const Vector &size, const Color &color) = 0;

        virtual void quadXZ(const Vector &position, const Vector &size, const Vector &texturePosition,
                            const Vector &textureSize, const Material &material) = 0;

        virtual void quadYZ(const Vector &position, const Vector &size, const Color &color) = 0;

        virtual void quadYZ(const Vector &position, const Vector &size, const Vector &texturePosition,
                            const Vector &textureSize, const Material &material) = 0;

        virtual void point(const Vector &position, Float32 size, const Color &color) = 0;

        virtual void line(const Vector &from, const Vector &to, Float32 width, const Color &color) = 0;

        virtual void frame(const Vector &position, const Vector &size, Float32 width, const Color &color) = 0;

        virtual std::unique_ptr<RendererBuffer> makeBuffer(const FaceList &faceList) = 0;

        virtual std::unique_ptr<RendererTarget> makeTarget(ScreenParameters screenParameters) = 0;

        virtual void stencil(StencilMode mode) const = 0;
    };
}

#endif
