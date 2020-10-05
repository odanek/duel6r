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
#include "Label.h"
namespace Duel6::Gui {

    class Dialog : public View {

        class DialogTitleBar : public Control {
        protected:
            Gui::Button *closeBt;
            Gui::Label *label;
            bool pressed = false;
            Int32 w, h;

        public:
            DialogTitleBar(Dialog &parentView, const std::string & title);

            void setPos(Int32 x, Int32 y, Int32 w, Int32 h);

            virtual void draw(Renderer &renderer, const Font &font) const override;

            virtual void mouseMotionEvent(const MouseMotionEvent &event) override;

            virtual void mouseButtonEvent(const MouseButtonEvent &event) override;

            virtual Type getType() const override {
                return Type::DialogTitlebar;
            }
        };

        friend class DialogTitleBar;

    public:
        enum RESIZE {
            NONE, N, E, S, W, NE, SE, SW, NW
        };

    private:

        DialogTitleBar *titleBar;

        RESIZE resizingDirection = NONE;

    public:
        Dialog(Desktop &parent, Int32 x, Int32 y, Int32 w, Int32 h, const std::string & title);

        virtual ~Dialog();

        virtual void draw(Renderer &renderer, const Font &font) const override;

        virtual void mouseButtonEvent(const MouseButtonEvent &event) override;

        virtual void mouseMotionEvent(const MouseMotionEvent &event) override;

    private:
        void startResizing(RESIZE direction);

        void stopResizing();
    };

} /* namespace Duel6 */

#endif /* SOURCE_GUI_DIALOG_H_ */
