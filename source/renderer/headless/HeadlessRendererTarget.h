/*
 * HeaderRendererTarget.h
 *
 *  Created on: May 13, 2020
 *      Author: fanick
 */

#ifndef SOURCE_RENDERER_HEADLESS_HEADERRENDERERTARGET_H_
#define SOURCE_RENDERER_HEADLESS_HEADERRENDERERTARGET_H_

#include "../RendererTarget.h"
namespace Duel6 {

    class HeadlessRendererTarget: public RendererTarget {
    public:
        void record(RenderCallback renderCallback) override;

        void apply(const Color &modulateColor) override;
    };

} /* namespace Duel6 */

#endif /* SOURCE_RENDERER_HEADLESS_HEADERRENDERERTARGET_H_ */
