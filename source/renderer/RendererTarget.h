#ifndef DUEL6_RENDERER_FRAMEBUFFER_H
#define DUEL6_RENDERER_FRAMEBUFFER_H

#include "RendererTypes.h"

namespace Duel6 {
    class RendererTarget {
    public:
    GLuint fbo;

    Texture texture;
    Texture depthTexture;
    GLuint width, height;
    public:
        RendererTarget (GLuint width, GLuint height);

        virtual ~RendererTarget ();

        void use();

        void stopUse();

        void clear();

        void blit();

    };
}
#endif
