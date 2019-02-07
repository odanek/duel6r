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
Projekt: Konzole (SDL verze)
Popis: Hlavni hlavickovy soubor
Potrebuje: Knihovnu mylib
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <vector>
#include <list>
#include <functional>
#include <memory>
#include <SDL2/SDL_keyboard.h>

#include "../Type.h"
#include "../Format.h"
#include "../Font.h"

#define CON_Lang(x)         x
#define CON_Format          Format

#define CON_VERSION         "5.7"
#define CON_TEXT_SIZE       32000
#define CON_MAX_ALIAS_REC   40
#define CON_REM_HIST        15

// Some default values
#define CON_DEF_SHOWLAST    10
#define CON_DEF_WIDTH       100
#define CON_TAB_WIDTH       3

// Klavesy
#define CON_C_SCROLL_UP     SDLK_PAGEUP
#define CON_C_SCROLL_DOWN   SDLK_PAGEDOWN
#define CON_C_HIST_UP       SDLK_UP
#define CON_C_HIST_DOWN     SDLK_DOWN
#define CON_C_INSERT        SDLK_INSERT
#define CON_C_LEFT          SDLK_LEFT
#define CON_C_RIGHT         SDLK_RIGHT
#define CON_C_HOME          SDLK_HOME
#define CON_C_END           SDLK_END
#define CON_C_BACK          SDLK_BACKSPACE
#define CON_C_DELETE        SDLK_DELETE
#define CON_C_TAB           SDLK_TAB
#define CON_C_ENTER         SDLK_RETURN

namespace Duel6 {
    class Console {
    public:
        enum Flags {
            NonFlag = 0x00,
            RegInfoFlag = 0x01,
            ExpandFlag = 0x02
        };

    public:
        class Arguments {
        private:
            std::vector<std::string> arguments;

        public:
            Size length() const {
                return arguments.size();
            }

            const std::string &get(Size index) const {
                return arguments[index];
            }

            void clear() {
                arguments.clear();
            }

            void add(const std::string &arg) {
                arguments.push_back(arg);
            }
        };

        typedef std::function<void(Console &console, const Arguments &arguments)> Command;

        class Variable {
        public:
            typedef std::unique_ptr<Variable> Pointer;

        public:
            enum Flags {
                NoneFlag = 0x00,
                ReadOnlyFlag = 0x01,
                ArchiveFlag = 0x02
            };

        public:
            virtual void setValue(const std::string &val) = 0;

            virtual std::string getValue() const = 0;

            virtual std::string getTypeName() const = 0;

            template<class T>
            static Pointer from(T &val);
        };

        class IntVariable
                : public Variable {
        private:
            Int32 &value;

        public:
            explicit IntVariable(Int32 &value)
                    : value(value) {}

            void setValue(const std::string &val) override;

            std::string getValue() const override;

            std::string getTypeName() const override;
        };

        class BoolVariable
                : public Variable {
        private:
            bool &value;

        public:
            explicit BoolVariable(bool &value)
                    : value(value) {}

            void setValue(const std::string &val) override;

            std::string getValue() const override;

            std::string getTypeName() const override;
        };

        class FloatVariable
                : public Variable {
        private:
            Float32 &value;

        public:
            explicit FloatVariable(Float32 &value)
                    : value(value) {}

            void setValue(const std::string &val) override;

            std::string getValue() const override;

            std::string getTypeName() const override;
        };

    public:
        class CommandRecord {
        private:
            std::string name;
            Command command;

        public:
            CommandRecord(const std::string &name, Command command)
                    : name(name), command(command) {}

            const std::string &getName() const {
                return name;
            }

            const Command &getCommand() const {
                return command;
            }
        };

        struct AliasRecord {
        private:
            std::string name;
            std::string command;

        public:
            AliasRecord(const std::string &name, const std::string &command)
                    : name(name), command(command) {}

            const std::string &getName() const {
                return name;
            }

            const std::string &getCommand() const {
                return command;
            }

            void setCommand(const std::string &command) {
                this->command = command;
            }
        };

        class VarRecord {
        private:
            std::string name;
            Uint32 flags;
            Variable::Pointer var;

        private:
            VarRecord(const VarRecord &record) = delete;

            VarRecord &operator=(const VarRecord &record) = delete;

        public:
            VarRecord(const std::string &name, Uint32 flags, Variable::Pointer &&var)
                    : name(name), flags(flags), var(std::forward<Variable::Pointer>(var)) {}

            VarRecord(VarRecord &&record)
                    : name(record.name), flags(record.flags), var(std::move(record.var)) {}

            VarRecord &operator=(VarRecord &&record) {
                name = record.name;
                flags = record.flags;
                var = std::move(record.var);
                return *this;
            }

            const std::string &getName() const {
                return name;
            }

            bool hasFlag(Uint32 flag) const {
                return (flags & flag) == flag;
            }

            std::string getValue() const {
                return var->getValue();
            }

            void setValue(const std::string &val) {
                var->setValue(val);
            }

            void printInfo(Console &console) const;
        };

    private:
        bool visible;                      // Je konzole viditelna/aktivni?
        std::vector<Uint8> text;                      // Textovy buffer
        int width;                        // Sirka konzoly ve znacich
        unsigned long bufpos;                       // Pozice v bufferu kam se tiskne
        bool buffull;                      // Uz byl buffer prerotovan? Je plny?
        bool insert;                       // Prepinani vkladani/prepisovani
        int curpos;                       // Pozice kursoru na radku
        int show;                         // Kolik poslednich radku ukazat
        int scroll;                       // O kolik radku je odskrolovano

        Uint32 flags;                        // Flagy

        std::vector<CommandRecord> cmds;            // Seznam procedur
        std::vector<VarRecord> vars;                   // Senam promenych
        std::vector<AliasRecord> aliases;             // Seznam aliasu

        std::string input;                        // Radek vstupu
        int inputscroll;                  // O kolik je se vstupem odskrolovano doprava
        std::string hist[CON_REM_HIST];           // Ulozena historie prikazu
        int histcnt;                      // Pocet ulozenych prikazu historie
        int histscroll;                   // O kolik je v historii odskrolovano

        Arguments arguments;                          // Argumenty funkce
        std::list<std::string> cbuf;                  // Prikazovy buffer
        int aliasloop;                    // Pocet provedenych aliasu (proti zacykleni)

    public:
        explicit Console(Uint32 flags);

        ~Console();

        void render(Int32 csX, Int32 csY, const Font &font);

        Console &print(const std::string &str);

        Console &printLine(const std::string &str);

        void keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers);

        void textInputEvent(const std::string &text);

        void toggle() {
            visible = !visible;
        }

        bool isActive() const {
            return visible;
        }

        void setLast(int sl);

        void registerCommand(const std::string &name, Command command);

        void registerVariable(const std::string &name, Uint32 flags, Variable::Pointer &&var);

        void registerAlias(const std::string &name, const std::string &cmd);

        const std::vector<CommandRecord> &listCommands() const {
            return cmds;
        }

        const std::vector<VarRecord> &listVars() const {
            return vars;
        }

        const std::vector<AliasRecord> &listAliases() const {
            return aliases;
        }

        const Uint8 *getText(Size &bufPos, bool &bufFull) const;

        void clear();

        Console &appendCommands(const std::string &commands);

        Console &prependCommands(const std::string &commands);

        void execute();

        void exec(const std::string &commands);

        static void registerBasicCommands(Console &c_ptr);

    private:
        void verifyRegistration(const std::string &proc, const std::string &name, bool isNull);

        std::string expandLine(const std::string &line);

        std::string::const_iterator
        nextToken(const std::string &line, std::string::const_iterator &begin, std::string::const_iterator &end);

        void tokenizeLine(const std::string &line, Arguments &args);

        void executeSingleLine(const std::string &line);

        void splitCommandsIntoLines(const std::string &commands, std::vector<std::string> &lines);

        void varCmd(VarRecord &var, Arguments &args);

        void setInputScroll();

        void completeCmd();

        void renderLine(int y, Size pos, Int32 len, const Font &font) const;

        void renderHistory(int csY, const Font &font) const;

        void renderBackground(Int32 csX, Int32 csY, const Font &font) const;

        void renderSeparator(Int32 csY, const Font &font) const;

        void renderInputLine(Int32 csY, const Font &font) const;

        CommandRecord *findCommand(const std::string &name);

        VarRecord *findVar(const std::string &name);

        AliasRecord *findAlias(const std::string &name);

        bool hasFlag(Uint32 flag) const {
            return (flags & flag) == flag;
        }
    };
}

#endif
