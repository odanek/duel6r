/*
 * View.cpp
 *
 *  Created on: Sep 11, 2020
 *      Author: fanick
 */

#include "View.h"
#include "Desktop.h"

namespace Duel6::Gui {
    namespace {
        Color frameLightColor(215, 215, 215);
        Color frameDarkColor(0, 0, 0);
        Color frameFocusColor(0, 50, 255, 50);
        Color bgColor(205, 205, 205);
        Color shadowColor(0, 0, 0, 158);
    }
    View::View(Desktop &parent, Int32 x, Int32 y, Int32 w, Int32 h)
        : parent(parent), x(x), y(y), w(w), h(h) {
        parent.addView(this);
    }

    View::~View() {
    }

    void View::focus(Control *control) {
        if (control != focused) {
            blur(focused);
        }
        focused = control;
        focused->setFocused(true);
    }

    void View::blur(Control *control) {
        if (focused != nullptr) {
            focused->onBlur();
            focused->setFocused(false);
            focused = nullptr;
        }
    }

    void View::focusPrevious() {
        advanceFocus(true);
    }

    void View::focusNext() {
        advanceFocus(false);
    }

    void View::addControl(Control *control) {
        control->setParent(this);
        controls.push_back(std::unique_ptr<Control>(control));
    }

    Font& View::getFont() {
        return parent.getFont();
    }

    void View::setIBeamCursor() {
        return parent.setIBeamCursor();
    }
    void View::setNWSECursor() {
        return parent.setNWSECursor();
    }
    void View::setNESWCursor() {
        return parent.setNESWCursor();
    }
    void View::setWECursor() {
        return parent.setWECursor();
    }
    void View::setNSCursor() {
        return parent.setNSCursor();
    }

    void View::setPos(Int32 x, Int32 y) {
        setPos(x, y, w, h);
    }

    void View::getPos(Int32 &x, Int32 &y) {
        x = this->x;
        y = this->y;
    }

    void View::getPos(Int32 &x, Int32 &y, Int32 &w, Int32 &h) {
        x = this->x;
        y = this->y;
        w = this->w;
        h = this->h;
    }

    void View::setPos(Int32 x, Int32 y, Int32 w, Int32 h) {
        if (w < 100) {
            w = 100;
        }
        if (h < 100) {
            h = 100;
        }
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;

        onResize();
    }

    void View::onResize(ResizeCallback callback) {
        resizeCallbacks.push_back(callback);
    }

    void View::onResize() {
        for (auto &callback : resizeCallbacks) {
            callback(*this, x, y, w, h);
        }
    }

    void View::advanceFocus(bool backwards) {
        Control *first = nullptr;
        Control *previous = (*controls.rbegin()).get();
        for (auto &control : controls) {
            if (!control->focusable) {
                continue;
            }
            if (first == nullptr && focused != control.get()) {
                first = control.get();
            }
            if (backwards) {
                if (focused == nullptr || focused == control.get()) {
                    focus(previous);
                    return;
                }
            } else {
                if (focused == nullptr || focused == previous) {
                    focus(control.get());

                    return;
                }
            }
            previous = control.get();
        }
        if (backwards) {
            if (previous != nullptr) {
                focus(previous);
            }
        } else {
            if (first != nullptr) {
                focus(first);
            }
        }
    }

    void View::draw(Renderer &renderer, const Font &font) const {
        Matrix m = renderer.getViewMatrix();
        renderer.setViewMatrix(m * Matrix::translate(Float32(x), Float32(y), 0));
        for (auto &control : controls) {
            control->draw(renderer, font);
        }
        renderer.setViewMatrix(m);
    }

    bool View::keyEvent(const KeyPressEvent &event) {
        if (event.getCode() == SDLK_TAB) {
            if (event.withShift()) {
                focusPrevious();
            } else {
                focusNext();
            }
            return true;
        }
        if (focused != nullptr) {
            if (focused->keyEvent(event)) {
                return true;
            }
            if (event.getCode() == SDLK_ESCAPE) {
                blur(focused);
                return true;
            }
        }
        return false;
    }

    void View::textInputEvent(const TextInputEvent &event) {
        if (focused != nullptr) {
            focused->textInputEvent(event);
        }
    }

    void View::mouseButtonEvent(const MouseButtonEvent &event) {
        MouseButtonEvent translatedEvent = event.translate(-x, -y);
        for (auto &control : controls) {
            control->mouseButtonEvent(translatedEvent);
        }
    }

    void View::mouseMotionEvent(const MouseMotionEvent &event) {
        MouseMotionEvent translatedEvent = event.translate(-x, -y);
        for (auto &control : controls) {
            control->mouseMotionEvent(translatedEvent);
        }
    }

    void View::mouseWheelEvent(const MouseWheelEvent &event) {
        MouseWheelEvent translatedEvent = event.translate(-x, -y);
        for (auto &control : controls) {
            control->mouseWheelEvent(translatedEvent);
        }
    }

    void View::close() {
        parent.closeView(this);
    }

    void View::drawDropShadow(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h, bool p, bool focus) {
        w++;
        h++;
        renderer.setBlendFunc(BlendFunc::SrcAlpha);
        Float32 shadowWidth = 16.0f;
        // bottom shadow
        renderer.line(Vector(x + 4, y - 6), Vector(x + w - 1, y - 6), shadowWidth, shadowColor);
        // right shadow
        renderer.line(Vector(x + w + 4, y + h - 8), Vector(x + w + 4, y - 11), shadowWidth, shadowColor);
        renderer.setBlendFunc(BlendFunc::None);
    }
    void View::drawBackground(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h, bool p, bool focus) {
        renderer.quadXY(Vector(x, y), Vector(w, h), bgColor);
    }
    void View::drawFrame(Renderer &renderer, Int32 x, Int32 y, Int32 w, Int32 h, bool p, bool focus) {
        w--;
        h--;

        const Color &topColor = p ? frameDarkColor : frameLightColor;
        const Color &bottomColor = p ? frameLightColor : frameDarkColor;

        renderer.line(Vector(x, y), Vector(x, y + h), 1.0f, topColor);
        renderer.line(Vector(x + 1, y), Vector(x + 1, y + h + 1), 1.0f, topColor);

        renderer.line(Vector(x, y), Vector(x + w, y), 1.0f, topColor);
        renderer.line(Vector(x, y - 1), Vector(x + w - 1, y - 1), 1.0f, topColor);

        renderer.line(Vector(x + w, y + h), Vector(x + w, y), 1.0f, bottomColor);
        renderer.line(Vector(x + w + 1, y + h), Vector(x + w + 1, y - 1), 1.0f, bottomColor);

        renderer.line(Vector(x + w, y), Vector(x, y), 1.0f, bottomColor);
        renderer.line(Vector(x + w, y + 1), Vector(x + 1, y + 1), 1.0f, bottomColor);

    }

} /* namespace Duel6 */
