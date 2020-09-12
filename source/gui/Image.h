/*
 * Image.h
 *
 *  Created on: Sep 11, 2020
 *      Author: fanick
 */

#ifndef DUEL6_GUI_IMAGE_H_
#define DUEL6_GUI_IMAGE_H_
#include "Control.h"
#include "../Material.h"
namespace Duel6::Gui {
    class View;

    class Image: Control {
    private:
        Material material;
        Int32 w, h;
    public:
        Image(View &parentView, Material material, Int32 x, Int32 y, Int32 w, Int32 h);
        virtual ~Image();

        virtual Type getType() const override {
            return Type::Image;
        }

    protected:
        virtual void draw(Duel6::Renderer &renderer, const Duel6::Font &font) const override;
    };

} /* namespace Duel6 */

#endif /* SOURCE_GUI_IMAGE_H_ */
