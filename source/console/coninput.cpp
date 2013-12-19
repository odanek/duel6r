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

#include <string.h>
#include "console.h"

/*
==================================================
Opravi pozici od ktere se zobrazuje prikazova radka
==================================================
*/
void con_c::setinputscroll (void)
{
    int     l = (int) strlen (m_input),
        width = m_width - 1;

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
void con_c::inpfound (char *name, int *fit, int *nf, char **ffName)
{
    static char *first;
    int         i;

    (*nf)++;
    if (*nf == 1)
        first = name;
    else
    {
        if (*nf == 2)
            printf (CON_Lang ("CONSTR0028|]Hledani: \"%s\"\n\t%s\n"), m_input, first);
        printf ("\t%s\n", name);
    }

    if (*ffName == NULL)
    {
        *fit = (int) strlen (name);
        *ffName = name;
    }
    else
    {
        i = 0;
        while (i < *fit && name[i])
            if (name[i] == (*ffName)[i])
                i++;
            else
                break;

        *fit = i;
    }
}

void con_c::completecmd (void)
{
    conAlias_s      *a;
    conCommand_s    *p;
    conVar_s        *v;
    char            *ffName = NULL;
    int             l = (int) strlen (m_input), fit = 0, nf = 0;

    if (!l)
        return;

    for (p = m_procs; p != NULL; p = p->next)
        if (!strncmp (m_input, p->name, l))
            inpfound (p->name, &fit, &nf, &ffName);
    for (v = m_vars; v != NULL; v = v->next)
        if (!strncmp (m_input, v->name, l))
            inpfound (v->name, &fit, &nf, &ffName);
    for (a = m_alias; a != NULL; a = a->next)
        if (!strncmp (m_input, a->name, l))
            inpfound (a->name, &fit, &nf, &ffName);

    if (nf)
    {
        memcpy (m_input, ffName, fit);
        if (nf == 1)
        {
            m_input[fit] = ' ';
            m_input[++fit] = 0;
        }
        else
            m_input[fit] = 0;
        m_curpos = fit;
    }
}

/*
==================================================
Reakce na stisk klavesy
==================================================
*/
void con_c::keyevent (int k)
{
    int len;

    if (k == '`')
    {
        m_visible = !m_visible;
        return;
    }

    if (!m_visible)
        return;

    switch (k)
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
            strcpy (m_input, m_hist[(m_histcnt - m_histscroll) % CON_REM_HIST]);
            m_curpos = (int) strlen (m_input);
        }
        break;
    case CON_C_HIST_DOWN:
        if (m_histscroll > 0)
        {
            m_histscroll--;
            if (!m_histscroll)
                m_input[0] = 0;
            else
                strcpy (m_input, m_hist[(m_histcnt - m_histscroll) % CON_REM_HIST]);
            m_curpos = (int) strlen (m_input);
        }
        break;
    case CON_C_INSERT:
        m_insert = !m_insert;
        break;
    default:
        len = (int) strlen (m_input);

        if (k == CON_C_BACK && m_curpos)
        {
            memmove (&m_input[m_curpos - 1], &m_input[m_curpos], 1 + len - m_curpos);
            len--;
            m_curpos--;
        }
        if (k == CON_C_DELETE && m_curpos < len)
        {
            memmove (&m_input[m_curpos], &m_input[m_curpos + 1], len - m_curpos);
            len--;
        }
        else if (k == 13 && len)
        {
            strcpy (m_hist[m_histcnt % CON_REM_HIST], m_input);
            m_histcnt++;
            m_histscroll = 0;
            printf ("]%s\n", m_input);
            exec (m_input);
            m_input[0] = 0;
            m_curpos = 0;
        }
        else if (k == CON_C_TAB)
            completecmd ();
        else if (len < CON_MAX_INPUT && k >= ' ' && k < 127)
        {
            if (!m_insert || m_curpos == len)
            {
                len++;
                memmove (&m_input[m_curpos + 1], &m_input[m_curpos], len - m_curpos);
                m_input[m_curpos++] = (char) k;
            }
            else
                m_input[m_curpos++] = (char) k;
        }
        else if (k == CON_C_LEFT && m_curpos > 0)
            m_curpos--;
        else if (k == CON_C_RIGHT && m_curpos < len)
            m_curpos++;
        else if (k == CON_C_HOME)
            m_curpos = 0;
        else if (k == CON_C_END)
            m_curpos = len;
        break;
    }

    setinputscroll ();
}
