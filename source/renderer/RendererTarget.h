#ifndef DUEL6_RENDERER_FRAMEBUFFER_H
#define DUEL6_RENDERER_FRAMEBUFFER_H

#include "RendererTypes.h"
#include "Renderer.h"
namespace Duel6 {
    class Renderer;
    class RendererTarget {
    public:
    GLuint fbo;

    Texture texture;
    Texture depthTexture;

    GLuint width, height;
    Renderer & renderer;

    public:
        RendererTarget (GLuint width, GLuint height, Renderer & renderer);

        virtual ~RendererTarget ();

        void use();

        void stopUse();

        void clear();

        void blit();

        void blitDepth();

        void render(Color & color);

        Texture getTexture();

        Texture getDepthTexture();

    };
}
#endif
