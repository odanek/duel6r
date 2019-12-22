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

#include "GL4RendererTarget.h"
#include "GL4Renderer.h"
#include "../../VideoException.h"

namespace Duel6 {
    GL4RendererTarget::GL4RendererTarget(GL4Renderer &renderer, ScreenParameters screenParameters)
            : width(screenParameters.getClientWidth()),
              height(screenParameters.getClientHeight()),
              renderer(renderer) {
        GLuint depthBufferFormat;
        switch (screenParameters.getDepthBits()) {
            case 16: depthBufferFormat = GL_DEPTH_COMPONENT16; break;
            case 24: depthBufferFormat = GL_DEPTH_COMPONENT24; break;
            default: depthBufferFormat = GL_DEPTH_COMPONENT; break;
        }

        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0, 0);
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, depthTexture);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, depthBufferFormat, width, height, 1, 0, GL_DEPTH_COMPONENT,
                     GL_UNSIGNED_INT, nullptr);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            D6_THROW(VideoException, "OpenGL error: Incomplete frameBuffer ");
        }
        GLenum att[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, att);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            D6_THROW(VideoException, "OpenGL error: Incomplete frameBuffer ");
        }
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GL4RendererTarget::~GL4RendererTarget() {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &texture);
        glDeleteTextures(1, &depthTexture);
    }

    void GL4RendererTarget::record(RenderCallback renderCallback) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        renderer.clearBuffers();
        renderCallback();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GL4RendererTarget::apply(const Color &modulateColor) {
        if (modulateColor != Color::WHITE) {
            blitDepth();
            render(modulateColor); // render texture with color blending (slower)
        } else {
            blit(); // faster
        }
    }

    void GL4RendererTarget::blit() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                          GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    void GL4RendererTarget::blitDepth() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void GL4RendererTarget::render(const Color &color) {
        renderer.quadXY(Vector::ZERO, Vector((float) width, (float) height), Vector(1, 1), Vector(1, 1),
                              Material::makeColoredTexture(texture, color));
    }
}
