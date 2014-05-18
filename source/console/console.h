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
#define CON_FSX             ((conWORD) m_font[0])
#define CON_FSY             ((conWORD) m_font[1])

// Variable fags
#define CON_FLAGS           2
#define CON_F_NONE          0x00000
#define CON_F_RONLY         0x00001
#define CON_F_ARCHIVE       0x00002

// Variable types
enum conVarType_e
{
    CON_VAR_FLOAT = 0,
    CON_VAR_INT,
    CON_VAR_BOOL
};

// Forward declaration
class con_c;

typedef void                (*conProc_t) (con_c *con);
typedef unsigned char       conBYTE;
typedef unsigned short      conWORD;

struct conCommand_s
{
    std::string name;
    conProc_t execute;
};

struct conAlias_s
{
    std::string name;
    std::string command;
};

struct conVar_s
{
    std::string name;
    void *ptr;
    int flags;
    conVarType_e type;
};

/*
==================================================
Trida con_c
==================================================
*/
class con_c
{
private:
    bool            m_visible;                      // Je konzole viditelna/aktivni?
    conBYTE         m_text[CON_TEXT_SIZE];          // Textovy buffer
    int             m_width;                        // Sirka konzoly ve znacich
    unsigned long   m_bufpos;                       // Pozice v bufferu kam se tiskne
    bool            m_buffull;                      // Uz byl buffer prerotovan? Je plny?
    bool            m_insert;                       // Prepinani vkladani/prepisovani
    int             m_curpos;                       // Pozice kursoru na radku
    int             m_show;                         // Kolik poslednich radku ukazat
    int             m_scroll;                       // O kolik radku je odskrolovano

    int             m_flags;                        // Flagy
    const conBYTE   *m_font;                        // Ukazatel na font (nahradit SDL_ttf?
    conProc_t       m_infoproc;                     // Procedura volana po printf

    std::vector<conCommand_s> m_procs;              // Seznam procedur
    std::vector<conVar_s> m_vars;                   // Senam promenych
    std::vector<conAlias_s> m_alias;                // Seznam aliasu

    std::string     m_input;                        // Radek vstupu
    int             m_inputscroll;                  // O kolik je se vstupem odskrolovano doprava
    std::string     m_hist[CON_REM_HIST];           // Ulozena historie prikazu
    int             m_histcnt;                      // Pocet ulozenych prikazu historie
    int             m_histscroll;                   // O kolik je v historii odskrolovano

    std::vector<std::string> m_argv;				// Argumenty funkce
    std::list<std::string> m_cbuf;					// Prikazovy buffer
    int             m_aliasloop;                    // Pocet provedenych aliasu (proti zacykleni)

public:
	con_c(int flags);
	~con_c();

    void blit(int res_x, int res_y);

    void printf(const char *str, ...);
    void keyEvent(SDL_Keycode keyCode);
	void textInputEvent(const char* text);
	
	void toggle()
	{
		m_visible = !m_visible;
	}

    bool isactive() 
	{ 
		return m_visible; 
	}

    void setlast(int sl);
    void setfont(const conBYTE *p);
    void setinfoproc(conProc_t p);

	const std::string& argv(size_t index) const;
    size_t argc() const;

    int regcmd(conProc_t p, const std::string& name);
    int regvar(void *p, const std::string& name, int flags, conVarType_e typ);
    int regalias(const std::string& name, const std::string& cmd);

	const std::vector<conCommand_s>& listCommands() const
	{
		return m_procs;
	}

	const std::vector<conVar_s>& listVars() const
	{
		return m_vars;
	}

	const std::vector<conAlias_s>& listAliases() const
	{
		return m_alias;
	}

    const conBYTE *gettext(unsigned long *buf_pos, bool *buf_full) const;
	void clear();

	con_c& appendCommands(const std::string& commands);
	con_c& prependCommands(const std::string& commands);
    void exec(const std::string& commands);
    void execute();

    void varInfo(const conVar_s& var);
    void setVarValue(conVar_s& var, const std::string& val);
    std::string getVarValue(const conVar_s& var) const;

private:
    int namevalid(const std::string& name, const char *proc, void *p);
    std::string expandLine(const std::string& line);
	std::string::const_iterator nextToken(const std::string& line, std::string::const_iterator& begin, std::string::const_iterator& end);
    void tokenizeLine(const std::string& line);
    void executeSingleLine(const std::string& line);
    void varCmd(conVar_s& var);

    void setinputscroll();
    void completecmd();

    void dprint_line(int y, unsigned long pos, int len);
    void dshow_hist(int res_y);
    void drawchar(int x, int y, int c);

    conCommand_s* findCommand(const std::string& name);
    conVar_s* findVar(const std::string& name);
    conAlias_s *findAlias(const std::string& name);

	void splitCommandsIntoLines(const std::string& commands, std::vector<std::string>& lines);
};

// Dodelat vsude podporu multijazykoveho prostredi aby ji mohly vyuzivat vsechny knihovny a program
void CON_RegisterBasicCmd(con_c *c_ptr);

#endif
