/*
 * Dialog.cpp
 *
 *  Created on: Sep 11, 2020
 *      Author: fanick
 */

#include "Dialog.h"
namespace Duel6::Gui {
    const auto EDGEWIDTH = 4;
    Dialog::DialogTitleBar::DialogTitleBar(Dialog &parentView, const std::string & title)
        : Control(parentView) {
        Int32 x, y, w, h;
        parent->getPos(x, y, w, h);
        label = new Gui::Label(parentView);
        label->setPosition(5, h - 8, w - 34, 16);
        label->setCaption(title);
        closeBt = new Gui::Button(parentView);
        closeBt->setPosition(w - 24, h - 8, 16, 16);
        closeBt->setCaption("x");
        closeBt->setFocusable(true);
        closeBt->onClick([this](Button &button) {
            parent->close();
        });

        parentView.onResize([this](View &dialog, Int32 x, Int32 y, Int32 w, Int32 h) {
            this->w = w;
            setPos(0, h, this->w, this->h);
            label->setPosition(5, h - 8, w - 34, 16);
            closeBt->setPosition(this->w - 24, h - 8, 16, 16);
        });

    }

    Dialog::Dialog(Desktop &parent, Int32 x, Int32 y, Int32 w, Int32 h, const std::string & title)
        : View(parent, x, y, w, h) {
        titleBar = new Dialog::DialogTitleBar(*this, title);
        titleBar->setPos(0, this->h, this->w, 32);
    }

    Dialog::~Dialog() {
    }

    void Dialog::draw(Renderer &renderer, const Font &font) const {
        drawBackground(renderer, x, y, w, h, false, false);
        drawFrame(renderer, x, y, w, h, false, false);
        drawDropShadow(renderer, x, y, w, h, false, false);

        View::draw(renderer, font);
    }

    void Dialog::DialogTitleBar::setPos(Int32 x, Int32 y, Int32 w, Int32 h) {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    void Dialog::DialogTitleBar::draw(Renderer &renderer, const Font &font) const {
        drawFrame(renderer, x, y, w, h, false, false);

    }

    void Dialog::DialogTitleBar::mouseMotionEvent(const MouseMotionEvent &event) {
        if (pressed) {
            Int32 px, py;
            parent->getPos(px, py);
            parent->setPos(px + event.getXDiff(), py + event.getYDiff());
        }
    }

    void Dialog::DialogTitleBar::mouseButtonEvent(const MouseButtonEvent &event) {
        if (pressed && !event.isPressed() && event.getButton() == SysEvent::MouseButton::LEFT) {
            pressed = false;
        }

        if (Control::mouseIn(event, x + EDGEWIDTH, y - EDGEWIDTH, w - 2 * EDGEWIDTH, h - 2 * EDGEWIDTH)) {
            if (event.getButton() == SysEvent::MouseButton::LEFT) {
                if (!pressed && event.isPressed()) {
                    pressed = true;
                } else
                if (pressed && !event.isPressed()) {
                    pressed = false;
                }
            }
        }
    }
    Dialog::RESIZE getResize(Int32 X, Int32 Y, Int32 w, Int32 h){

        if (Y > h - EDGEWIDTH) {
            if (X > w - EDGEWIDTH) {
                return Dialog::RESIZE::NE;
            } else
            if (X < EDGEWIDTH) {
                return Dialog::RESIZE::NW;
            }
            return  Dialog::RESIZE::N;
        } else
        if (X > w - EDGEWIDTH) {
            if (Y < EDGEWIDTH) {
                return Dialog::RESIZE::SE;
            }
            return Dialog::RESIZE::E;
        } else
        if (Y < EDGEWIDTH) {
            if (X < EDGEWIDTH) {
                return Dialog::RESIZE::SW;
            }
            return Dialog::RESIZE::S;
        } else
        if (X < EDGEWIDTH) {
            return Dialog::RESIZE::W;
        }
        return Dialog::RESIZE::NONE;
    }
    void Dialog::mouseButtonEvent(const MouseButtonEvent &event) {

        View::mouseButtonEvent(event);
      //  MouseButtonEvent translatedEvent = event.translate(-x, -y);
        if(resizingDirection != NONE && event.getButton() == SysEvent::MouseButton::LEFT && !event.isPressed()){
            stopResizing();
        }

        Int32 X = event.getX() - x;
        Int32 Y = event.getY() - y;
        if (View::mouseIn(event, x, y, w, h)) {
            if (event.getButton() == SysEvent::MouseButton::LEFT) {
                if (event.isPressed()) {
                    startResizing(getResize(X,Y,w,h));
                }
            }
        }
    }

    void Dialog::mouseMotionEvent(const MouseMotionEvent &event) {
        View::mouseMotionEvent(event);

        Int32 X = event.getX() - x;
        Int32 Y = event.getY() - y;
        if (View::mouseIn(event, x, y, w, h)) {
            RESIZE r = getResize(X,Y,w,h);
            if(r != NONE){
                switch(r){
                case N:
                case S:
                    setNSCursor();
                    break;
                case E:
                case W:
                    setWECursor();
                    break;
                case NE:
                case SW:
                    setNESWCursor();
                    break;
                case SE:
                case NW:
                    setNWSECursor();
                    break;
                default:
                break;
                }
            }
        }

        if (resizingDirection == NONE) {
            return;
        }
        Int32 dx = 0, dy = 0, dw = 0, dh = 0;
        Int32 xdiff = event.getXDiff();
        Int32 ydiff = event.getYDiff();
        switch (resizingDirection) {
        case (N): {
            dh += ydiff;
            break;
        }
        case (E): {
            dw += xdiff;
            break;
        }
        case (S): {
            dh -= ydiff;
            dy += ydiff;
            break;
        }
        case (W): {
            dw -= xdiff;
            dx += xdiff;
            break;
        }
        case (NE): {
            dh += ydiff;
            dw += xdiff;
            break;
        }
        case (SE): {
            dh -= ydiff;
            dy += ydiff;
            dw += xdiff;
            break;
        }
        case (SW): {
            dh -= ydiff;
            dy += ydiff;
            dw -= xdiff;
            dx += xdiff;
            break;
        }
        case (NW): {
            dh += ydiff;
            dw -= xdiff;
            dx += xdiff;
            break;
        }
        default:
            break;
        }

        setPos(x + dx, y + dy, w + dw, h + dh);
    }

    void Dialog::startResizing(RESIZE direction) {
        resizingDirection = direction;
    }

    void Dialog::stopResizing() {
        resizingDirection = NONE;
    }

} /* namespace Duel6 */
