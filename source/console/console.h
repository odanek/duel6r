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

#ifndef __CONSOLE_H
#define __CONSOLE_H

#ifndef __MYLIB_H
#include "../mylib/mylib.h"
#endif

#define CON_GetMem          MY_Alloc
#define CON_Free            MY_Free
#define CON_Lang            MY_L

#define CON_VERSION         "5.3.1"
#define CON_TEXT_SIZE       32000
#define CON_MAX_INPUT       300
#define CON_MAX_CBUF        8192
#define CON_MAX_PSTR        2048
#define CON_MAX_ARGS        50
#define CON_MAX_NAME        50
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
#define CON_C_SCROLL_UP     280
#define CON_C_SCROLL_DOWN   281
#define CON_C_HIST_UP       273
#define CON_C_HIST_DOWN     274
#define CON_C_INSERT        277
#define CON_C_LEFT          276
#define CON_C_RIGHT         275
#define CON_C_HOME          278
#define CON_C_END           279
#define CON_C_BACK          8
#define CON_C_DELETE        127
#define CON_C_TAB           9

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
    char                    *name;
    conProc_t               execute;
    struct conCommand_s     *next;
};

struct conAlias_s
{
    char                *name;
    char                *text;
    struct conAlias_s   *next;
};

struct conVar_s
{
    char            *name;
    void            *ptr;
    int             flags;
    conVarType_e    type;
    struct conVar_s *next;
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

    conCommand_s    *m_procs;                       // Seznam procedur
    conVar_s        *m_vars;                        // Senam promenych
    conAlias_s      *m_alias;                       // Seznam aliasu

    char            m_input[CON_MAX_INPUT + 2];     // Radek vstupu
    int             m_inputscroll;                  // O kolik je se vstupem odskrolovano doprava
    char            m_hist[CON_REM_HIST][CON_MAX_INPUT + 2];  // Ulozena historie prikazu
    int             m_histcnt;                      // Pocet ulozenych prikazu historie
    int             m_histscroll;                   // O kolik je v historii odskrolovano

    char            *m_argv[CON_MAX_ARGS];          // Argumenty funkce
    int             m_argc;                         // Pocet argumentu funkce

    char            m_cbuf[CON_MAX_CBUF];           // Prikazovy buffer
    int             m_cbuflen;                      // Delka prikazoveho bufferu
    int             m_aliasloop;                    // Pocet provedenych aliasu (proti zacykleni)

public:
            con_c       (int flags);
            ~con_c      ();

    void    blit        (int res_x, int res_y);

    void    printf      (const char *str, ...);
    void    keyevent    (int k);

    void    setlast     (int sl);
    void    setfont     (const conBYTE *p);
    void    setinfoproc (conProc_t p);

    char    *argv       (int l);
    int     argc        (void);

    int     regcmd      (conProc_t p, const char *name);
    int     regvar      (void *p, const char *name, int flags, conVarType_e typ);
    int     regalias    (const char *alias, const char *cmd);

    const conCommand_s  *getcmd     (const char *name) const;
    const conVar_s      *getvar     (const char *name) const;
    const conAlias_s    *getalias   (const char *name) const;
    const conBYTE       *gettext    (unsigned long *buf_pos, bool *buf_full) const;

    void    addcmd      (const char *cmd);
    void    insertcmd   (const char *cmd);
    void    exec        (const char *s);
    void    execute     (void);
    void    clear       (void);
    bool    isactive    (void) { return m_visible; }
    void    varinfo     (const conVar_s *v);

    void    var_set     (const char *name, const char *val);
    char    *var_get    (const conVar_s *v, char *val);

private:
    int     namevalid   (const char *name, const char *proc, void *p);
    void    expand      (char *tx);
    int     gettoken    (char *t, int *s);
    void    tokenize    (char *tx);
    void    execline    (char *line);
    bool    varcmd      (void);

    void    setinputscroll      (void);
    void    inpfound            (char *name, int *fit, int *nf, char **ffName);
    void    completecmd         (void);

    void    dprint_line         (int y, unsigned long pos, int len);
    void    dshow_hist          (int res_y);
    void    drawchar            (int x, int y, int c);
};

// Dodelat vsude podporu multijazykoveho prostredi aby ji mohly vyuzivat vsechny knihovny a program
void    CON_RegisterBasicCmd    (con_c *c_ptr);

#endif
