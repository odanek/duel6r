/*
 * HeadlessRenderer.cpp
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

#include "../../FaceList.h"
#include "../../Exception.h"

#include "HeadlessRenderer.h"
#include "HeadlessRendererTarget.h"
namespace Duel6 {
    Renderer::Info HeadlessRenderer::getInfo() {
        Info info;
        info.vendor = "duel6 laboratories";
        info.renderer = "headless renderer";
        info.version = "1.0";
        return info;
    }

    Renderer::Extensions HeadlessRenderer::getExtensions() {
        Extensions info;
        return info;
    }

    Texture HeadlessRenderer::createTexture(const Image &image, TextureFilter filtering, bool clamp) {
        Texture textureId;
        return textureId;
    }

    void HeadlessRenderer::freeTexture(Texture textureId) {
    }

    Image HeadlessRenderer::makeScreenshot() {
        auto width = 64;
        auto height = 64;
        Image image(width, height);
        return image.flipY();
    }

    void HeadlessRenderer::setViewport(Int32 x, Int32 y, Int32 width, Int32 height) {

    }

    void HeadlessRenderer::enableWireframe(bool enable) {

    }

    void HeadlessRenderer::enableDepthTest(bool enable) {

    }

    void HeadlessRenderer::enableDepthWrite(bool enable) {

    }

    void HeadlessRenderer::setBlendFunc(BlendFunc func) {

    }

    void HeadlessRenderer::setGlobalTime(Float32 time) {

    }

    void HeadlessRenderer::clearBuffers() {

    }

    void HeadlessRenderer::setProjectionMatrix(const Matrix &m) {

    }

    void HeadlessRenderer::setViewMatrix(const Matrix &m) {

    }

    void HeadlessRenderer::setModelMatrix(const Matrix &m) {

    }

    void HeadlessRenderer::point(const Vector &position, Float32 size, const Color &color) {

    }

    void HeadlessRenderer::line(const Vector &from, const Vector &to, Float32 width, const Color &color) {

    }

    void HeadlessRenderer::triangle(const Vector &p1, const Vector &p2, const Vector &p3, const Color &color) {

    }

    void HeadlessRenderer::triangle(const Vector &p1, const Vector &t1,
                               const Vector &p2,
                               const Vector &t2,
                               const Vector &p3,
                               const Vector &t3,
                               const Material &material) {

    }

    void HeadlessRenderer::quad(const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, const Color &color) {

    }

    void HeadlessRenderer::quad(const Vector &p1, const Vector &t1,
                           const Vector &p2,
                           const Vector &t2,
                           const Vector &p3,
                           const Vector &t3,
                           const Vector &p4,
                           const Vector &t4,
                           const Material &material) {

    }

    std::unique_ptr<RendererBuffer> HeadlessRenderer::makeBuffer(const FaceList &faceList) {
        return std::make_unique<HeadlessBuffer>();
    }

    std::unique_ptr<RendererTarget> HeadlessRenderer::makeTarget(ScreenParameters screenParameters) {
        return std::make_unique<HeadlessRendererTarget>();
    }

    void HeadlessRenderer::enableOption(GLenum option, bool enable) {
    }

    void HeadlessRenderer::updateColorBuffer(Int32 vertexCount) {
    }

    void HeadlessRenderer::updateMaterialBuffer(Int32 vertexCount) {
    }

    void HeadlessRenderer::updateMvpUniform() {
    }

} /* namespace Duel6 */
