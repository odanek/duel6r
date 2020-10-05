/*
 * HeadlessRenderer.h
 *
 *  Created on: May 13, 2020
 *      Author: fanick
 */


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
#ifndef SOURCE_RENDERER_HEADLESS_HEADLESSRENDERER_H_
#define SOURCE_RENDERER_HEADLESS_HEADLESSRENDERER_H_

#include "HeadlessTypes.h"
#include "../RendererBase.h"
#include "HeadlessBuffer.h"

namespace Duel6 {

    class HeadlessRenderer : public RendererBase {

    public:
        HeadlessRenderer() = default;

        ~HeadlessRenderer() = default;

        Info getInfo() override;

        Extensions getExtensions() override;

        Texture createTexture(const Image &image, TextureFilter filtering, bool clamp) override;

        void freeTexture(Texture textureId) override;

        Image makeScreenshot() override;

        void setViewport(Int32 x, Int32 y, Int32 width, Int32 height) override;

        void enableWireframe(bool enable) override;

        void enableDepthTest(bool enable) override;

        void enableDepthWrite(bool enable) override;

        void setBlendFunc(BlendFunc func) override;

        void setGlobalTime(Float32 time) override;

        void clearBuffers() override;

        void setProjectionMatrix(const Matrix &m) override;

        void setViewMatrix(const Matrix &m) override;

        void setModelMatrix(const Matrix &m) override;

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

        std::unique_ptr<RendererBuffer> makeBuffer(const FaceList &faceList) override;

        std::unique_ptr<RendererTarget> makeTarget(ScreenParameters screenParameters) override;

    private:
        void enableOption(GLenum option, bool enable);

        void updateColorBuffer(Int32 vertexCount);

        void updateMaterialBuffer(Int32 vertexCount);

        void updateMvpUniform();
    };
}

#endif /* SOURCE_RENDERER_HEADLESS_HEADLESSRENDERER_H_ */
