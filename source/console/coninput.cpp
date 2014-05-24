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

/*
==================================================
Opravi pozici od ktere se zobrazuje prikazova radka
==================================================
*/
void con_c::setinputscroll ()
{
    int     l = (int)m_input.length(), width = m_width - 1;

    if (m_curpos == l)
    {
        m_inputscroll = (l + 2) - m_width;
        if (m_inputscroll < 0)
            m_inputscroll = 0;
        return;
    }

    if (l <= width)
    {
        m_inputscroll = 0;
        return;
    }

    if (l - m_inputscroll < width)
        m_inputscroll = l - width;

    if (m_curpos < m_inputscroll)
        m_inputscroll = m_curpos;

    if (m_curpos >= m_inputscroll + width)
        m_inputscroll = 1 + m_curpos - width;
}

/*
==================================================
Dohledani prikazu
==================================================
*/
static bool startsWith(const std::string& text, const std::string& prefix)
{
	return prefix.length() <= text.length() && text.compare(0, prefix.length(), prefix) == 0;
}

void con_c::completecmd ()
{
	if (m_input.empty())
	{
		return;
	}

	std::vector<std::string> fittingCommands;
	for (const conCommand_s& command : m_procs)
	{
		if (startsWith(command.name, m_input))
		{
			fittingCommands.push_back(command.name);
		}
	}
	for (const conVar_s& var : m_vars)
	{
		if (startsWith(var.name, m_input))
		{
			fittingCommands.push_back(var.name);
		}
	}
	for (const conAlias_s& alias : m_alias)
	{
		if (startsWith(alias.name, m_input))
		{
			fittingCommands.push_back(alias.name);
		}
	}

	if (fittingCommands.size() == 1)
	{
		m_input = fittingCommands.front();
		m_input.append(" ");
		m_curpos = m_input.length();
	}
	else if (fittingCommands.size() > 1)
	{
        printf(CON_Lang("CONSTR0028|]Hledani: \"%s\"\n"), m_input.c_str());
		for (const std::string& command : fittingCommands)
		{
			printf("\t%s\n", command.c_str());
		}

		std::string largestFit = fittingCommands.front();
		for (const std::string& command : fittingCommands)
		{
			size_t prefix = m_input.length();
			while (prefix < largestFit.length() && prefix < command.length() && largestFit[prefix] == command[prefix])
			{
				++prefix;
			}

			if (prefix < largestFit.length())
			{
				largestFit = largestFit.substr(0, prefix);
			}
		}

		m_input = largestFit;
		m_curpos = m_input.length();
	}
}

/*
==================================================
Reakce na stisk klavesy
==================================================
*/
void con_c::keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers)
{
    int len = (int)m_input.length();

    switch (keyCode)
    {
    case CON_C_SCROLL_UP:
        m_scroll++;
        break;
    case CON_C_SCROLL_DOWN:
        if (m_scroll > 0)
            m_scroll--;
        break;
    case CON_C_HIST_UP:
        if (m_histscroll < CON_REM_HIST && m_histcnt - m_histscroll > 0)
        {
            m_histscroll++;
			m_input = m_hist[(m_histcnt - m_histscroll) % CON_REM_HIST];
            m_curpos = (int)m_input.length();
        }
        break;
    case CON_C_HIST_DOWN:
        if (m_histscroll > 0)
        {
            m_histscroll--;
			if (!m_histscroll)
			{
				m_input.clear();
			}
			else
			{
				m_input = m_hist[(m_histcnt - m_histscroll) % CON_REM_HIST];
			}
            m_curpos = (int) m_input.length();
        }
        break;
    case CON_C_INSERT:
        m_insert = !m_insert;
        break;
	case CON_C_BACK:
		if (m_curpos > 0)
        {
			m_input.erase(m_input.begin() + m_curpos - 1);
            m_curpos--;
        }
		break;
	case CON_C_DELETE:
		if (m_curpos < len)
        {
			m_input.erase(m_input.begin() + m_curpos);
        }
		break;
	case CON_C_ENTER: 
		if (len)
        {
            m_hist[m_histcnt % CON_REM_HIST] = m_input;
            m_histcnt++;
            m_histscroll = 0;
            printf ("]%s\n", m_input.c_str());
            exec(m_input);
            m_input.clear();
            m_curpos = 0;
        }
		break;
	case CON_C_TAB:
        completecmd();
		break;
	case CON_C_LEFT:
		if (m_curpos > 0)
		{
			m_curpos--;
		}
		break;
	case CON_C_RIGHT: 
		if (m_curpos < len)
		{
			m_curpos++;
		}
		break;
	case CON_C_HOME:
        m_curpos = 0;
		break;
	case CON_C_END:
        m_curpos = len;
        break;
    }

    setinputscroll ();
}

void con_c::textInputEvent(const char* text)
{
	while (*text != 0)
	{
		if (*text >= ' ' && *text < 128 && *text != '`')
		{
			if (!m_insert || m_curpos == (int)m_input.length())
			{
				m_input.insert(m_input.begin() + m_curpos, *text);
				++m_curpos;
			}
			else
			{
				m_input[m_curpos++] = *text;
			}
		}

		text++;
	}
}
