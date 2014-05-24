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
Popis: Hlavni funkce
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "console.h"

///////////////////////////////////////////////////////
//                Zakladni funkce                    //
///////////////////////////////////////////////////////

/*
==================================================
Inicializace/deinicializace konzole
==================================================
*/
con_c::con_c (int flags)
{
    m_visible = false;
    m_insert = false;
    m_curpos = 0;
    m_inputscroll = 0;
    m_flags = flags;

    m_input.clear();
    m_histcnt = 0;
    m_histscroll = 0;
    m_font = NULL;
    m_infoproc = NULL;

    m_width = CON_DEF_WIDTH;
    m_show = CON_DEF_SHOWLAST;

    clear();

    printf("==========================\n");
    printf(CON_Lang("CONSTR0029|Konzole -- autor O.Danek\n"));
    printf(CON_Lang("CONSTR0030|Verze %s\n"), CON_VERSION);
    printf("==========================\n");
}

con_c::~con_c ()
{
	m_vars.clear();
	m_procs.clear();
	m_alias.clear();
}

/*
==================================================
Vyprazdneni textoveho bufferu
==================================================
*/
void con_c::clear ()
{
    m_bufpos = 0;
    m_scroll = 0;
    m_buffull = false;
    memset (m_text, '\n', CON_TEXT_SIZE);
}

/*
==================================================
Formatovany tisk do konzoly
V infoproc by se nemelo s konzolou nic delat
==================================================
*/
void con_c::printf (const char *str, ...)
{
    char    fstr[CON_MAX_PSTR + 1], *tx;
    va_list argptr;

    tx = fstr;

    va_start (argptr, str);
    vsprintf (tx, str, argptr);
    va_end (argptr);

    while (*tx)
    {
        switch (*tx)
        {
        case '\n':
            m_text[m_bufpos++] = *tx;
            break;
        case '\t':
            for (int i = 0; i < CON_TAB_WIDTH; i++, m_bufpos++)
                m_text[m_bufpos % CON_TEXT_SIZE] = ' ';
            break;
        default  :
            if (*tx >= ' ')
                m_text[m_bufpos++] = *tx;
            break;
        }

        if (m_bufpos >= CON_TEXT_SIZE)
        {
            m_bufpos -= CON_TEXT_SIZE;
            m_buffull = true;
        }
        tx++;
    }

    m_scroll = 0;

    if (m_infoproc != NULL)
        m_infoproc (this);
}

///////////////////////////////////////////////////////
//                  Registrace                       //
///////////////////////////////////////////////////////

/*
==================================================
Kontrola pripustnosti jmena
==================================================
*/
int con_c::namevalid (const std::string& name, const char *proc, void *p)
{
	if (name.empty())
    {
        printf (CON_Lang("CONSTR0032|%s : nazev \"%s\" ma neplatnou delku\n"), proc, name.c_str());
        return CON_FAILED;
    }

    if (findCommand(name) != nullptr || findVar(name) != nullptr || findAlias(name) != nullptr)
    {
        printf (CON_Lang("CONSTR0033|%s : nazev \"%s\" uz je registrovan\n"), proc, name.c_str());
        return CON_ALREADY;
    }

    if (p == nullptr)
    {
        printf (CON_Lang("CONSTR0034|%s : \"%s\" s NULL ukazatelem\n"), proc, name.c_str());
        return CON_FAILED;
    }

    return CON_SUCCES;
}

/*
==================================================
Registrovani prikazu
==================================================
*/
int con_c::regcmd (conProc_t p, const std::string& name)
{
    int i = namevalid (name, CON_Lang("CONSTR0035|Registrace prikazu"), (void *) p);
    if (i != CON_SUCCES)
        return i;

	conCommand_s newCommand;
	newCommand.name = name;
	newCommand.execute = p;

    // Zaradi novy prikaz tak, aby byly setridene podle abecedy
	size_t position = 0;
    for (const conCommand_s& command : m_procs)
    {
		if (command.name.compare(newCommand.name) > 0)
		{
			break;
		}
		++position;
    }

	m_procs.insert(m_procs.begin() + position, newCommand);

	if (m_flags & CON_F_REG_INFO)
	{
		printf(CON_Lang("CONSTR0036|Registrace prikazu: \"%s\" na adrese 0x%p byla uspesna\n"), name.c_str(), p);
	}

    return CON_SUCCES;
}

/*
==================================================
Registrace aliasu
==================================================
*/
int con_c::regalias (const std::string& name, const std::string& cmd)
{
    conAlias_s *a = findAlias(name);

    if (a == NULL)
    {
        int i = namevalid (name, CON_Lang("CONSTR0037|Registrace aliasu"), (void *) (1));
        if (i != CON_SUCCES)
            return i;

		conAlias_s newAlias;
		newAlias.name = name;
		newAlias.command = cmd;

        // Zaradi novy alias tak, aby byly setridene podle abecedy
		size_t position = 0;
		for (const conAlias_s& alias : m_alias)
        {
			if (alias.name.compare(name) > 0)
			{
				break;
			}
			++position;
        }

		m_alias.insert(m_alias.begin() + position, newAlias);
    }
	else
	{
		a->command = cmd;
	}

	if (m_flags & CON_F_REG_INFO)
	{
		printf(CON_Lang("CONSTR0038|Registrace aliasu: \"%s\" za \"%s\" byla uspesna\n"), name.c_str(), cmd.c_str());
	}

    return CON_SUCCES;
}

/*
==================================================
Vraceni ukazatele na prikaz
==================================================
*/
conCommand_s *con_c::findCommand(const std::string& name)
{
	for (conCommand_s& command : m_procs)
	{
		if (command.name == name)
		{
			return &command;
		}
	}

    return nullptr;
}

/*
==================================================
Vraci ukazatel na alias s danym jmenem
==================================================
*/
conAlias_s *con_c::findAlias(const std::string& name)
{
	for (conAlias_s& alias : m_alias)
	{
		if (alias.name == name)
		{
			return &alias;
		}
	}

    return nullptr;
}

///////////////////////////////////////////////////////
//                Dalsi funkce                       //
///////////////////////////////////////////////////////

/*
==================================================
Nastaveni procedury ktera bude volana
po kazdem con_c::printf jako notifikace
==================================================
*/
void con_c::setinfoproc (conProc_t p)
{
    m_infoproc = p;
}

/*
==================================================
Nastaveni ukazatele na font
==================================================
*/
void con_c::setfont (const conBYTE *p)
{
    m_font = p;
}

/*
==================================================
Nastaveni kolik predchozich radku bude zobrazeno
==================================================
*/
void con_c::setlast (int sl)
{
    m_show = sl > 1 ? sl : 2;
}

/*
==================================================
Vraci ukazatel na textovy buffer konzole
==================================================
*/
const conBYTE *con_c::gettext (unsigned long *buf_pos, bool *buf_full) const
{
    *buf_pos = m_bufpos;
    *buf_full = m_buffull;
    return m_text;
}
