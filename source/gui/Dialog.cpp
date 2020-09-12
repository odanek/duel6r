/*
 * Dialog.cpp
 *
 *  Created on: Sep 11, 2020
 *      Author: fanick
 */

#include "Dialog.h"
namespace Duel6::Gui {

    Dialog::Dialog(Desktop &parent, Int32 x, Int32 y, Int32 w, Int32 h)
        : View(parent, x, y, w, h)
    {
        titleBar = new Dialog::DialogTitleBar(*this);
        titleBar->setPos(0, this->h, this->w, 32);
        closeBt = new Gui::Button(*this);
        closeBt->setPosition(this->w - 24, this->h - 8, 16, 16);
        closeBt->setCaption("x");
        closeBt->setFocusable(true);
        closeBt->onClick([this](Button &button) {
            this->close();
        });
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

        if (Control::mouseIn(event, x, y, w, h)) {
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
} /* namespace Duel6 */
