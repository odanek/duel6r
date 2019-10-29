#include "RendererTarget.h"
#include "../Exception.h"
#include <sstream>
namespace Duel6 {
    RendererTarget::RendererTarget(GLuint width, GLuint height)
        : width(width),
          height(height) {

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT,  GL_UNSIGNED_INT, NULL);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            GLenum e = glGetError();
            D6_THROW(Exception, "incomplete frameBuffer ");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

}
