#include "RendererTarget.h"
#include "../Exception.h"
#include <sstream>

#define GLCHECK {auto e = glGetError(); if(e != GL_NO_ERROR){ D6_THROW(Exception, (std::string("errorrr ") + std::to_string(e)).c_str());}}

namespace Duel6 {
    RendererTarget::RendererTarget(GLuint width, GLuint height, Renderer & renderer)
        : width(width),
          height(height),
          renderer(renderer){

        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);



        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GLCHECK
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        GLCHECK
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLCHECK
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLCHECK
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0, 0);
        GLCHECK
        glGenTextures(1, &depthTexture);
        GLCHECK
        glBindTexture(GL_TEXTURE_2D_ARRAY, depthTexture);
        GLCHECK
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT24, width, height, 1, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        GLCHECK
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0, 0);
        GLCHECK
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            D6_THROW(Exception, "incomplete frameBuffer ");
        }
        GLCHECK
        GLenum att[1] = {GL_COLOR_ATTACHMENT0};//,GL_DEPTH_ATTACHMENT};
        glDrawBuffers(1, att);
        GLCHECK
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            D6_THROW(Exception, "incomplete frameBuffer ");
        }
        GLCHECK
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        GLCHECK
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GLCHECK
    }

    RendererTarget::~RendererTarget() {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &texture);
        glDeleteTextures(1, &depthTexture);
    }

    void RendererTarget::use() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }


    void RendererTarget::stopUse() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RendererTarget::clear() {
        use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RendererTarget::blit() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT  |  GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    void RendererTarget::blitDepth() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    }

    void RendererTarget::render(Color & color) {
        this->renderer.quadXY(Vector::ZERO, Vector((float)width, (float)height), Vector(1, 1), Vector(1, 1), Material::makeColoredTexture(texture, color));
    }
}
