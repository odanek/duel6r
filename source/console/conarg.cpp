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
Popis: Zpracovani argumentu a prikazoveho bufferu
*/

#include <stdio.h>
#include <string.h>
#include "console.h"

static char conNullStr[3] = { 0, 0, 0 };

/*
==================================================
Vraci dany argument funkce
==================================================
*/
char *con_c::argv (int l)
{
    if (l < 0 || l >= m_argc)
        return conNullStr;

    return m_argv[l];
}

/*
==================================================
Vraci pocet argumentu funkce
==================================================
*/
int con_c::argc (void)
{
    return m_argc;
}

/*
==================================================
Expanduje promene v retezci
==================================================
*/
void con_c::expand (char *tx)
{
    const conVar_s  *v;
    char            value[30];
    int             i = 0, se, l, l2;

    l = (int) strlen (tx) + 1;

    while (tx[i])
    {
        se = l2 = 0;

        while (tx[i])
        {
            if (tx[i] == '$' && tx[i + 1] == '{' && !l2)
                se = i + 2;

            if (tx[i] == '}' && se)
                break;

            if (tx[i] == '"')
                l2 ^= 0x01;

            i++;
        }

        if (!tx[i])
            return;

        if (i == se)
            continue;

        tx[i] = 0;
        v = getvar (&tx[se]);
        tx[i] = '}';

        if (v == NULL)
            continue;

        var_get (v, value);

        i++;
        se -= 2;
        l2 = (int) strlen (value);

        if (se + l - i + l2 < 1024)
        {
            memmove (&tx[se + l2], &tx[i], l - i);
            memcpy (&tx[se], value, l2);
            l = se + l - i + l2;
            i = se + l2;
        }
    }
}

/*
==================================================
Nalezne dalsi token v retezci
==================================================
*/
int con_c::gettoken (char *t, int *s)
{
    int     i = 0, u, l;

    while (true)
    {
        while (t[i] == '"' && t[i + 1] == '"')
            i += 2;

        if (t[i] > ' ')
            break;
        else if (!t[i])
            return 0;

        i++;
    }

    if (t[i] == '/' && t[i + 1] == '/')
        return 0;

    *s = i;
    u = 0x00;
    l = (int) strlen (t);

    while (true)
    {
        while (t[i] == '"')
        {
            memmove (&t[i], &t[i + 1], l - i);
            u ^= 0x1;
            l--;
        }

        if (t[i] <= ' ' - u)
            break;

        i++;
    }

    return i - *s;
}

/*
==================================================
Prevod retezce na tokeny
==================================================
*/
void con_c::tokenize (char *tx)
{
    int     start, len;

    m_argc = 0;
    if (m_flags & CON_F_EXPAND)
        expand (tx);

    while (*tx)
    {
        len = gettoken (tx, &start);

        if (!len)
            return;

        m_argv[m_argc++] = &tx[start];

        if (!tx[start + len])
            return;

        tx[start + len] = 0;
        tx += start + len + 1;

        if (m_argc >= CON_MAX_ARGS)
            return;
    }
}

/*
==================================================
Provedeni jedne radky command bufferu
==================================================
*/
void con_c::execline (char *line)
{
    const conCommand_s  *p;
    const conAlias_s    *a;

    if (!strlen (line))
        return;

    tokenize (line);

    if (m_argc > 0)
    {
        p = getcmd (m_argv[0]);
        if (p != NULL)
        {
            p->execute (this);
            return;
        }

        a = getalias (m_argv[0]);
        if (a != NULL)
        {
            if (++m_aliasloop == CON_MAX_ALIAS_REC)
                printf (CON_Lang("CONSTR0001|CON_Error : Zacykleni alias rekurze, zbyvajici aliasy jsou ignorovany\n"));
            else if (m_aliasloop < CON_MAX_ALIAS_REC)
                insertcmd (a->text);
            return;
        }

        if (!varcmd ())
            printf (CON_Lang("CONSTR0002|Neznamy prikaz : \"%s\"\n"), m_argv[0]);
    }
}

/*
==================================================
Pridani prikazu na konec command bufferu
==================================================
*/
void con_c::addcmd (const char *cmd)
{
    int     l = (int) strlen (cmd);

    if (!l)
        return;

    if (m_cbuflen + l >= CON_MAX_CBUF)
    {
        printf (CON_Lang("CONSTR0003|CON_Error : preteceni bufferu, prikaz ignorovan\n"));
        return;
    }

    memcpy (&m_cbuf[m_cbuflen], cmd, l);
    m_cbuflen += l;

    if (cmd[l - 1] != '\n')
        m_cbuf[m_cbuflen++] = '\n';
}

/*
==================================================
Pridani prikazu na zacetek command bufferu
==================================================
*/
void con_c::insertcmd (const char *cmd)
{
    int     l = (int) strlen (cmd), n = 0;

    if (!l)
        return;

    if (!m_cbuflen)
    {
        addcmd (cmd);
        return;
    }

    if (m_cbuflen + l >= CON_MAX_CBUF)
    {
        printf (CON_Lang("CONSTR0004|CON_Error : preteceni bufferu, vlozeni ignorovano\n"));
        return;
    }

    if (cmd[l - 1] != '\n')
        n = l++;

    memmove (m_cbuf + l, m_cbuf, m_cbuflen);
    memcpy (m_cbuf, cmd, l);
    m_cbuflen += l;

    if (n)
        m_cbuf[n] = '\n';
}

/*
==================================================
Provede obsah command bufferu
==================================================
*/
void con_c::execute (void)
{
    int     i, zv;
    char    line[1024], *text;

    text = m_cbuf;
    m_aliasloop = 0;

    while (m_cbuflen)
    {
        for (i = 0, zv = 0; i < m_cbuflen; i++)
        {
            if (text[i] == '"')
                zv ^= 1;
            if (text[i] == '\n' || (!zv && text[i] == ';'))
                break;
            if (i > 1021)
            {
                printf (CON_Lang("CONSTR0005|CON_Error : preteceni pri provadeni, buffer vyprazdnen\n"));
                m_cbuflen = 0;
                return;
            }
        }

        memcpy (line, text, i);
        line[i] = 0;

        if (i >= m_cbuflen - 1)
            m_cbuflen = 0;
        else
        {
            i++;
            m_cbuflen -= i;
            memmove (text, text + i, m_cbuflen);
        }

        execline (line);
    }
}

/*
==================================================
Prida prika do command bufferu a zavola Execute
==================================================
*/
void con_c::exec (const char *s)
{
    addcmd (s);
    execute ();
}
