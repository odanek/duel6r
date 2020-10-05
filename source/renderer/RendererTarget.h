#ifndef DUEL6_RENDERER_TARGET_H
#define DUEL6_RENDERER_TARGET_H

#include "RendererTypes.h"
#include "Renderer.h"

namespace Duel6 {
    class RendererTarget {
    public:
        using RenderCallback = std::function<void()>;

    public:
        virtual ~RendererTarget() = default;
        virtual void record(RenderCallback renderCallback) = 0;
        virtual void apply(const Color& modulateColor) = 0;
        virtual void resize(Int32 width, Int32 height) {};
    };
}
#endif
