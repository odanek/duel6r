/*
 * View.h
 *
 *  Created on: Sep 11, 2020
 *      Author: fanick
 */

#ifndef DUEL6_GUI_VIEW_H_
#define DUEL6_GUI_VIEW_H_

#include <vector>
#include "Control.h"

namespace Duel6::Gui {
    class Desktop;
    class View {
    private:
        Desktop &parent;
        std::vector<std::unique_ptr<Control>> controls;
        Control *focused = nullptr;
    protected:
        Int32 x = 0,y = 0, w = 100,h = 100;
    public:
        View(Desktop &parent, Int32 x = 0,Int32 y = 0, Int32 w = 100, Int32 h = 100);
        virtual ~View();

        void focus(Control *control);

        void blur(Control *control);

        void focusNext();

        void focusPrevious();

        void setPos(Int32 x, Int32 y){
            this->x = x;
            this->y = y;
        }

        void getPos(Int32 & x, Int32 & y){
            x = this->x;
            y = this->y;
        }
        void setPos(Int32 x, Int32 y, Int32 w, Int32 h){
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        }
        virtual void update(Float32 elapsedTime) {
            for (auto &control : controls) {
                control->update(elapsedTime);
            }
        }

        virtual void draw(Renderer &renderer, const Font &font) const;

        virtual bool keyEvent(const KeyPressEvent &event);

        virtual void textInputEvent(const TextInputEvent &event);

        virtual void mouseMotionEvent(const MouseMotionEvent &event);

        virtual void mouseButtonEvent(const MouseButtonEvent &event);

        virtual void mouseWheelEvent(const MouseWheelEvent &event);

        virtual void onBlur() {}

        void addControl(Control *control);

        Font& getFont();

        void setIBeamCursor();

        void close();
    protected:
           static void drawFrame(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h, bool p, bool focus = false);
           static void drawDropShadow(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h, bool p, bool focus = false);
           static void drawBackground(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h, bool p, bool focus = false);

    private:

        void advanceFocus(bool backwards = false);
    }
    ;

} /* namespace Duel6 */

#endif /* SOURCE_GUI_VIEW_H_ */
