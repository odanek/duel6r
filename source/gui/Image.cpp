/*
 * Image.cpp
 *
 *  Created on: Sep 11, 2020
 *      Author: fanick
 */

#include "Image.h"

namespace Duel6 {
    namespace Gui {

        Image::Image(View & parentView, Material material, Int32 x, Int32 y, Int32 w, Int32 h)
            : Control(parentView), material(material), w(w), h(h) {
            this->x = x;
            this->y = y;
        }

        Image::~Image(){
        }

        void Image::draw(Duel6::Renderer &renderer, const Duel6::Font &font) const {
            renderer.quadXY(Vector(x, y), Vector(w, h), Vector(0, 1), Vector(1, -1), material);
        }

    } /* namespace Gui */
} /* namespace Duel6 */
