/*
 * ChatInput.h
 *
 *  Created on: Oct 17, 2020
 *      Author: fanick
 */

#ifndef SOURCE_CHATINPUT_H_
#define SOURCE_CHATINPUT_H_

#include <string>

namespace Duel6 {
    class Renderer;
    class Font;
    class KeyPressEvent;
    class Input;

    class ChatInput {
    private:
        bool visible = false;
        std::string text = "";
        Input &input;
    public:

        ChatInput(Input &input);

        bool active() const {
            return visible;
        }

        bool toggle();

        void clear() {
            text.clear();
        }

        const std::string & getText() {
            return text;
        }
        virtual ~ChatInput(){

        }

        void render(Renderer &renderer, const Font &font) const;

        void textInputEvent(const std::string &text);

        void keyEvent(const KeyPressEvent &keyCode);
    };

} /* namespace Duel6 */

#endif /* SOURCE_CHATINPUT_H_ */
