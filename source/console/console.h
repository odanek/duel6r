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
#include <SDL2/SDL_keyboard.h>
#include "../mylib/mylib.h"

#define CON_Lang            MY_L

#define CON_VERSION         "5.4"
#define CON_TEXT_SIZE       32000
#define CON_MAX_PSTR        2048
#define CON_MAX_ALIAS_REC   40
#define CON_REM_HIST        15

// Some default values
#define CON_DEF_SHOWLAST    10
#define CON_DEF_WIDTH       100
#define CON_TAB_WIDTH       3

enum
{
    CON_SUCCES,
    CON_FAILED,
    CON_ALREADY
};

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
#define CON_C_ENTER			SDLK_RETURN

// Console flags
#define CON_F_REG_INFO      0x00001
#define CON_F_EXPAND        0x00002

// Velikost fontu
#define CON_FSX             ((conWORD) font[0])
#define CON_FSY             ((conWORD) font[1])

// Variable fags
#define CON_FLAGS           2
#define CON_F_NONE          0x00000
#define CON_F_RONLY         0x00001
#define CON_F_ARCHIVE       0x00002

typedef unsigned char       conBYTE;
typedef unsigned short      conWORD;

class Console
{
public:
	class Arguments
	{
	private:
		std::vector<std::string> arguments;

	public:
		size_t length() const
		{
			return arguments.size();
		}

		const std::string& get(size_t index) const
		{
			return arguments[index];
		}

		void clear()
		{
			arguments.clear();
		}

		void add(const std::string& arg)
		{
			arguments.push_back(arg);
		}
	};

	typedef void (*Callback) (Console& console, const Console::Arguments& arguments);

	class Command
	{
	public:
		std::string name;
		Callback callback;
	};

	struct Alias
	{
		std::string name;
		std::string command;
	};

	class Variable
	{
	public:
		enum class Type
		{
			Float,
			Int,
			Bool
		};

	public:
		std::string name;
		void *ptr;
		int flags;
		Type type;

	public:
		void printInfo(Console& console) const;
		void setValue(const std::string& val);
		std::string getValue() const;
	};

private:
    bool            visible;                      // Je konzole viditelna/aktivni?
    std::vector<conBYTE> text;					// Textovy buffer
    int             width;                        // Sirka konzoly ve znacich
    unsigned long   bufpos;                       // Pozice v bufferu kam se tiskne
    bool            buffull;                      // Uz byl buffer prerotovan? Je plny?
    bool            insert;                       // Prepinani vkladani/prepisovani
    int             curpos;                       // Pozice kursoru na radku
    int             show;                         // Kolik poslednich radku ukazat
    int             scroll;                       // O kolik radku je odskrolovano

    int             flags;                        // Flagy
    const conBYTE   *font;                        // Ukazatel na font (nahradit SDL_ttf?

    std::vector<Command> cmds;					  // Seznam procedur
    std::vector<Variable> vars;                   // Senam promenych
    std::vector<Alias> aliases;					  // Seznam aliasu

    std::string     input;                        // Radek vstupu
    int             inputscroll;                  // O kolik je se vstupem odskrolovano doprava
    std::string     hist[CON_REM_HIST];           // Ulozena historie prikazu
    int             histcnt;                      // Pocet ulozenych prikazu historie
    int             histscroll;                   // O kolik je v historii odskrolovano

    Arguments arguments;							// Argumenty funkce
    std::list<std::string> cbuf;					// Prikazovy buffer
    int             aliasloop;                    // Pocet provedenych aliasu (proti zacykleni)

public:
	Console(int flags);
	~Console();

    void blit(int res_x, int res_y);

    void printf(const char *str, ...);
    void keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers);
	void textInputEvent(const char* text);
	
	void toggle()
	{
		visible = !visible;
	}

    bool isActive() const
	{ 
		return visible; 
	}

    void setLast(int sl);
    void setFont(const conBYTE *p);

    int registerCommand(Callback callback, const std::string& name);
    int registerVariable(void *p, const std::string& name, int flags, Variable::Type type);
    int registerAlias(const std::string& name, const std::string& cmd);

	const std::vector<Command>& listCommands() const
	{
		return cmds;
	}

	const std::vector<Variable>& listVars() const
	{
		return vars;
	}

	const std::vector<Alias>& listAliases() const
	{
		return aliases;
	}

    const conBYTE *getText(unsigned long *buf_pos, bool *buf_full) const;
	void clear();

	Console& appendCommands(const std::string& commands);
	Console& prependCommands(const std::string& commands);
    void execute();
	void exec(const std::string& commands);

private:
    int isNameValid(const std::string& name, const char *proc, void *p);
    std::string expandLine(const std::string& line);
	std::string::const_iterator nextToken(const std::string& line, std::string::const_iterator& begin, std::string::const_iterator& end);
    void tokenizeLine(const std::string& line, Arguments& args);
    void executeSingleLine(const std::string& line);
    void varCmd(Variable& var, Arguments& args);

    void setInputScroll();
    void completeCmd();

    void dprintLine(int y, unsigned long pos, int len) const;
    void dshowHist(int res_y) const;
    void drawChar(int x, int y, int c) const;

    Command* findCommand(const std::string& name);
    Variable* findVar(const std::string& name);
    Alias* findAlias(const std::string& name);

	void splitCommandsIntoLines(const std::string& commands, std::vector<std::string>& lines);
};

// Dodelat vsude podporu multijazykoveho prostredi aby ji mohly vyuzivat vsechny knihovny a program
void CON_RegisterBasicCmd(Console& c_ptr);

#endif
