/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
Projekt: Konzole
Popis: Zpracovani prikazove radky
*/

#include "console.h"

namespace Duel6 {
    void Console::setInputScroll() {
        int inputLength = (int) input.length(), w = width - 1;

        if (curpos == inputLength) {
            inputscroll = (inputLength + 2) - width;
            if (inputscroll < 0)
                inputscroll = 0;
            return;
        }

        if (inputLength <= w) {
            inputscroll = 0;
            return;
        }

        if (inputLength - inputscroll < w)
            inputscroll = inputLength - w;

        if (curpos < inputscroll)
            inputscroll = curpos;

        if (curpos >= inputscroll + w)
            inputscroll = 1 + curpos - w;
    }

    static bool startsWith(const std::string &text, const std::string &prefix) {
        return prefix.length() <= text.length() && text.compare(0, prefix.length(), prefix) == 0;
    }

    void Console::completeCmd() {
        if (input.empty()) {
            return;
        }

        std::vector<std::string> fittingCommands;
        for (const CommandRecord &command : cmds) {
            if (startsWith(command.getName(), input)) {
                fittingCommands.push_back(command.getName());
            }
        }
        for (const VarRecord &var : vars) {
            if (startsWith(var.getName(), input)) {
                fittingCommands.push_back(var.getName());
            }
        }
        for (const AliasRecord &alias : aliases) {
            if (startsWith(alias.getName(), input)) {
                fittingCommands.push_back(alias.getName());
            }
        }

        if (fittingCommands.size() == 1) {
            input = fittingCommands.front();
            input.append(" ");
            curpos = (int) input.length();
        } else if (fittingCommands.size() > 1) {
            print(CON_Format(CON_Lang("Searching: \"{0}\"\n")) << input);
            for (const std::string &command : fittingCommands) {
                print(CON_Format("\t{0}\n") << command);
            }

            std::string largestFit = fittingCommands.front();
            for (const std::string &command : fittingCommands) {
                size_t prefix = input.length();
                while (prefix < largestFit.length() && prefix < command.length() &&
                       largestFit[prefix] == command[prefix]) {
                    ++prefix;
                }

                if (prefix < largestFit.length()) {
                    largestFit = largestFit.substr(0, prefix);
                }
            }

            input = largestFit;
            curpos = (int) input.length();
        }
    }

    /*
    ==================================================
    Reakce na stisk klavesy
    ==================================================
    */
    void Console::keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers) {
        int len = (int) input.length();

        switch (keyCode) {
            case CON_C_SCROLL_UP:
                scroll++;
                break;
            case CON_C_SCROLL_DOWN:
                if (scroll > 0)
                    scroll--;
                break;
            case CON_C_HIST_UP:
                if (histscroll < CON_REM_HIST && histcnt - histscroll > 0) {
                    histscroll++;
                    input = hist[(histcnt - histscroll) % CON_REM_HIST];
                    curpos = (int) input.length();
                }
                break;
            case CON_C_HIST_DOWN:
                if (histscroll > 0) {
                    histscroll--;
                    if (!histscroll) {
                        input.clear();
                    } else {
                        input = hist[(histcnt - histscroll) % CON_REM_HIST];
                    }
                    curpos = (int) input.length();
                }
                break;
            case CON_C_INSERT:
                insert = !insert;
                break;
            case CON_C_BACK:
                if (curpos > 0) {
                    input.erase(input.begin() + curpos - 1);
                    curpos--;
                }
                break;
            case CON_C_DELETE:
                if (curpos < len) {
                    input.erase(input.begin() + curpos);
                }
                break;
            case CON_C_ENTER:
                if (len) {
                    hist[histcnt % CON_REM_HIST] = input;
                    histcnt++;
                    histscroll = 0;
                    print(CON_Format("]{0}\n") << input);
                    exec(input);
                    input.clear();
                    curpos = 0;
                }
                break;
            case CON_C_TAB:
                completeCmd();
                break;
            case CON_C_LEFT:
                if (curpos > 0) {
                    curpos--;
                }
                break;
            case CON_C_RIGHT:
                if (curpos < len) {
                    curpos++;
                }
                break;
            case CON_C_HOME:
                curpos = 0;
                break;
            case CON_C_END:
                curpos = len;
                break;
        }

        setInputScroll();
    }

    void Console::textInputEvent(const std::string &text) {
        for (auto iter = text.cbegin(); iter != text.cend(); ++iter) {
            unsigned char letter = *iter;
            if (letter >= ' ' && letter < 128 && letter != '`') {
                if (!insert || curpos == (int) input.length()) {
                    input.insert(input.begin() + curpos, letter);
                    ++curpos;
                } else {
                    input[curpos++] = letter;
                }
            }
        }
    }
}