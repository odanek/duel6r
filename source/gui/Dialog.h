/*
 * Dialog.h
 *
 *  Created on: Sep 11, 2020
 *      Author: fanick
 */

#ifndef DUEL6_GUI_DIALOG_H_
#define DUEL6_GUI_DIALOG_H_

#include "View.h"
#include "Button.h"
namespace Duel6::Gui {

    class Dialog : public View {

        class DialogTitleBar : public Control {
            bool pressed = false;
            Int32 w, h;

        public:
            DialogTitleBar(Dialog &parentView)
                : Control(parentView) {
            }

            void setPos(Int32 x, Int32 y, Int32 w, Int32 h);

            virtual void draw(Renderer &renderer, const Font &font) const override;

            virtual void mouseMotionEvent(const MouseMotionEvent &event) override;

            virtual void mouseButtonEvent(const MouseButtonEvent &event) override;

            virtual Type getType() const override {
                return Type::DialogTitlebar;
            }
        };

        friend class DialogTitleBar;

    private:
        Gui::Button *closeBt;
        DialogTitleBar *titleBar;

    public:
        Dialog(Desktop &parent, Int32 x, Int32 y, Int32 w, Int32 h);
        virtual ~Dialog();

        virtual void draw(Renderer &renderer, const Font &font) const override;
    };

} /* namespace Duel6 */

#endif /* SOURCE_GUI_DIALOG_H_ */
