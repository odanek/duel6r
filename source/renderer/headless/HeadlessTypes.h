/*
 * HeadlessTypes.h
 *
 *  Created on: May 13, 2020
 *      Author: fanick
 */

#ifndef SOURCE_RENDERER_HEADLESS_HEADLESSTYPES_H_
#define SOURCE_RENDERER_HEADLESS_HEADLESSTYPES_H_

namespace Duel6 {
    typedef unsigned int Texture;
    typedef unsigned int GLenum; //todo
    enum class BlendFunc {
        None,
        SrcAlpha,
        SrcColor
    };

    enum class TextureFilter {
        Nearest,
        Linear
    };
}

#endif /* SOURCE_RENDERER_HEADLESS_HEADLESSTYPES_H_ */
