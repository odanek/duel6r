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
Popis: Zakladni prikazy pro beznou praci s konzolou
*/

#include <stdio.h>
#include <string.h>
#include "console.h"

/*
==================================================
Prikaz clear
==================================================
*/
static void CON_CmdClear (con_c *c_ptr)
{
    c_ptr->clear ();
}

/*
==================================================
Prikaz echo
==================================================
*/
static void CON_CmdEcho (con_c *c_ptr)
{
    int i, c = c_ptr->argc ();

    for (i = 1; i < c; i++)
        c_ptr->printf ("%s ", c_ptr->argv (i));

    c_ptr->printf ("\n");
}

/*
==================================================
Prikaz list
==================================================
*/
static void CON_CmdList (con_c *c_ptr)
{
    const conCommand_s  *cc;
    const conVar_s      *cv;
    char                *v1 = c_ptr->argv (0);
    int                 i = 0;

    if (c_ptr->argc () != 2)
    {
        c_ptr->printf (CON_Lang("CONSTR0006|%s : Pouziti %s <vars | cmds>\n"), v1, v1);
        return;
    }

    if (!strcmp (c_ptr->argv (1), "vars"))
    {
        c_ptr->printf (CON_Lang("CONSTR0007|Registrovane promenne :\n"));
        for (cv = c_ptr->getvar (NULL); cv != NULL; cv = cv->next, i++)
        {
            c_ptr->printf ("\t");
            c_ptr->varinfo (cv);
        }

        if (!i)
            c_ptr->printf (CON_Lang("CONSTR0008|\tNejsou registrovany zadne promenne\n"));
        else
            c_ptr->printf (CON_Lang("CONSTR0009|\n\t%d promennych celkove\n"), i);

        return;
    }

    if (!strcmp (c_ptr->argv (1), "cmds"))
    {
        c_ptr->printf (CON_Lang("CONSTR0010|Registrovane prikazy :\n"));
        for (cc = c_ptr->getcmd (NULL); cc != NULL; cc = cc->next, i++)
            c_ptr->printf ("\t\"%s\"\n", cc->name);

        if (!i)
            c_ptr->printf (CON_Lang("CONSTR0011|\tZadne prikazy nebyly registrovany\n"));
        else
            c_ptr->printf (CON_Lang("CONSTR0012|\n\t%d prikazu celkem\n"), i);

        return;
    }

    c_ptr->printf (CON_Lang("CONSTR0013|%s : Pouziti %s <vars | cmds>\n"), v1, v1);
}

/*
==================================================
Prikaz dump
==================================================
*/
static void CON_CmdDump (con_c *c_ptr)
{
    FILE            *f;
    unsigned long   i, bufpos;
    bool            bufful;
    const conBYTE   *buf;
    char            *v1 = c_ptr->argv (0),
                    *v2 = c_ptr->argv (1);

    if (c_ptr->argc () != 2)
    {
        c_ptr->printf (CON_Lang("CONSTR0014|%s : Pouziti %s <soubor>\n"), v1, v1);
        return;
    }

    f = fopen (v2, "wb");
    if (f == NULL)
    {
        c_ptr->printf (CON_Lang("CONSTR0015|%s : Nelze otevrit soubor %s\n"), v1, v2);
        return;
    }

    buf = c_ptr->gettext (&bufpos, &bufful);
    i = bufful ? CON_TEXT_SIZE : bufpos;
    bufpos = bufful ? bufpos : 0;

    while (i-- > 0)
    {
        fwrite (&buf[bufpos], 1, 1, f);
        if (++bufpos >= CON_TEXT_SIZE)
            bufpos -= CON_TEXT_SIZE;
    }

    fclose (f);

    c_ptr->printf (CON_Lang("CONSTR0016|Obsah konzole ulozen do souboru %s\n"), v2);
}

/*
==================================================
Prikaz parse
==================================================
*/
static void CON_CmdParse (con_c *c_ptr)
{
    FILE    *f;
    int     s;
    char    *v1 = c_ptr->argv (0),
            *v2 = c_ptr->argv (1), *t, o[100];

    if (c_ptr->argc () != 2)
    {
        c_ptr->printf (CON_Lang("CONSTR0017|%s : Pouziti %s <soubor>\n"), v1, v1);
        return;
    }

    f = fopen (v2, "rb");
    if (f == NULL)
    {
        c_ptr->printf (CON_Lang("CONSTR0018|%s : Nelze otevrit soubor %s\n"), v1, v2);
        return;
    }

    c_ptr->printf (CON_Lang("CONSTR0019|Provadeni souboru %s\n"), v2);
    fseek (f, 0, SEEK_END);
    s = ftell (f);
    if (!s)
        return;

    t = (char *) CON_GetMem (s + 1);
    fseek (f, 0, SEEK_SET);
    fread (t, 1, s, f);
    fclose (f);
    t[s] = 0;

    sprintf (o, CON_Lang("CONSTR0020|echo Konec provadeni %s"), v2);
    c_ptr->insertcmd (o);
    c_ptr->insertcmd (t);
    CON_Free (t);
}

/*
==================================================
Prikaz alias
==================================================
*/
static void CON_CmdAlias (con_c *c_ptr)
{
    const conAlias_s    *a;
    char                text[1024];
    int                 i, c;

    if (c_ptr->argc () == 1)
    {
        c_ptr->printf (CON_Lang("CONSTR0021|Registrovane prikazy alias :\n"));
        for (a = c_ptr->getalias (NULL); a != NULL; a = a->next)
            c_ptr->printf ("\t\"%s\" : %s\n", a->name, a->text);
        if (c_ptr->getalias (NULL) == NULL)
            c_ptr->printf (CON_Lang("CONSTR0022|\tZadne\n"));
        return;
    }

    text[0] = 0;
    c = c_ptr->argc ();

    for (i = 2; i < c; i++)
    {
        strcat (text, c_ptr->argv (i));
        if (i != c - 1)
            strcat (text, " ");
    }

    c_ptr->regalias (c_ptr->argv (1), text);
}

/*
==================================================
Prikaz archive
==================================================
*/
static void CON_CmdArchive (con_c *c_ptr)
{
    const conVar_s  *v;
    int             i = 0;
    FILE            *f;
    char            *v1 = c_ptr->argv (0), varval[30];

    if (c_ptr->argc () != 2)
    {
        c_ptr->printf (CON_Lang("CONSTR0023|%s : Pouziti %s <soubor>\n"), v1, v1);
        return;
    }

    f = fopen (c_ptr->argv (1), "wt");
    if (f == NULL)
    {
        c_ptr->printf (CON_Lang("CONSTR0024|%s : Nelze otevrit soubor\n"), v1);
        return;
    }

    c_ptr->printf (CON_Lang("CONSTR0025|Archivuji :\n"));
    fprintf (f, "%s", CON_Lang("CONSTR0026|//Generovano prikazem archive\n//Nemente rucne\n\n"));
    for (v = c_ptr->getvar (NULL); v != NULL; v = v->next)
        if (v->flags & CON_F_ARCHIVE)
        {
             i++;
             c_ptr->printf ("\t\"%s\"\n", v->name);
             fprintf (f, "\"%s\" %s\n", v->name, c_ptr->var_get (v, varval));
        }

    fclose (f);

    if (!i)
        c_ptr->printf (CON_Lang("CONSTR0027|\tZadna promenna nema archive flag\n"));
}

/*
==================================================
Registrace zakladnich prikazu
==================================================
*/
void CON_RegisterBasicCmd (con_c *c_ptr)
{
    c_ptr->regcmd (&CON_CmdClear, "clear");
    c_ptr->regcmd (&CON_CmdEcho, "echo");
    c_ptr->regcmd (&CON_CmdList, "list");
    c_ptr->regcmd (&CON_CmdDump, "dump");
    c_ptr->regcmd (&CON_CmdParse, "exec");
    c_ptr->regcmd (&CON_CmdAlias, "alias");
    c_ptr->regcmd (&CON_CmdArchive, "archive");
}
