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
Popis: Zpracovani promenych
*/

#include <stdlib.h>
#include <string.h>
#include "console.h"

/*
==================================================
Zaregistrovani promene
==================================================
*/
int con_c::regvar (void *p, const char *name, int flags, conVarType_e typ)
{
    int         i;
    conVar_s    *var, *last, *cur;

    i = namevalid (name, CON_Lang("CONSTR0039|Regitrace promenne"), p);
    if (i != CON_SUCCES)
        return i;

    var = (conVar_s *) CON_GetMem (sizeof (conVar_s));
    var->name = (char *) CON_GetMem ((int) strlen (name) + 1);
    strcpy (var->name, name);
    var->ptr = p;
    var->flags = flags;
    var->type = typ;

    // Zaradi novou promenou tak, aby byly setridene podle abecedy
    last = NULL;
    for (cur = m_vars; cur != NULL; cur = cur->next)
    {
        if (strcmp (var->name, cur->name) <= 0)
            break;
        last = cur;
    }

    var->next = cur;
    if (last != NULL)
        last->next = var;
    else
        m_vars = var;

    if (m_flags & CON_F_REG_INFO)
        printf (CON_Lang("CONSTR0040|Registrace promenne: \"%s\" na adrese 0x%p byla uspesna\n"), name, p);

    return CON_SUCCES;
}

/*
==================================================
Vraci ukazatel na danou promenou
Pokude je name NULL vraci ukazatel na seznam vsech promenych
==================================================
*/
const conVar_s *con_c::getvar (const char *name) const
{
    conVar_s    *p;

    if (name == NULL)
        return m_vars;

    for (p = m_vars; p != NULL; p = p->next)
        if (!strcmp (p->name, name))
            return p;

    return NULL;
}

/*
==================================================
Nastaveni hodnoty promene nebo vypsani hodnoty
na konzolu
==================================================
*/
bool con_c::varcmd (void)
{
    int         c = argc ();
    char        *name = argv (0);
    conVar_s    *v;

    for (v = m_vars; v != NULL; v = v->next)
        if (!strcmp (v->name, name))
            break;

    if (v == NULL)
        return false;

    if (c > 2)
    {
        printf (CON_Lang("CONSTR0041|Promenne : Pouziti jmeno_promenne [nova_hodnota]\n"));
        return true;
    }

    if (c == 1)
        varinfo (v);
    else
        if (!(v->flags & CON_F_RONLY))
            var_set (name, argv (1));
        else
            printf (CON_Lang("CONSTR0042|Promenna \"%s\" je pouze pro cteni\n"), v->name);

    return true;
}

/*
==================================================
Vytiskne na konzolu info o promene
==================================================
*/
void con_c::varinfo (const conVar_s *v)
{
    const char  flagstr[CON_FLAGS + 1] = "ra",
                *typestr[3] = { "float", "int", "bool" };
    char        varval[30];
    int         i, f = 1;

    if (v == NULL)
        return;

    for (i = 0; i < CON_FLAGS; i++, f <<= 1)
        if (v->flags & f)
            printf ("%c", flagstr[i]);
        else
            printf ("-");

    var_get (v, varval);
    printf (CON_Lang("CONSTR0043| %-5s \"%s\" s hodnotou %s\n"), typestr[v->type], v->name, varval);
}

/*
==================================================
Nastavi hodnotu promene podle retezce val
==================================================
*/
void con_c::var_set (const char *name, const char *val)
{
    conVar_s    *v;

    for (v = m_vars; v != NULL; v = v->next)
        if (!strcmp (v->name, name))
            break;

    if (v == NULL)
        return;

    switch (v->type)
    {
    case CON_VAR_FLOAT: *((float *)v->ptr) = (float) atof (val); break;
    case CON_VAR_INT: *((int *)v->ptr) = atoi (val); break;
    case CON_VAR_BOOL: *((bool *)v->ptr) = !strcmp (val, "true"); break;
    }
}

/*
==================================================
Ulozi hodnotu promene do retezce val
==================================================
*/
char *con_c::var_get (const conVar_s *v, char *val)
{
    switch (v->type)
    {
    case CON_VAR_FLOAT: sprintf (val, "%f", *((float *)v->ptr)); break;
    case CON_VAR_INT: sprintf (val, "%d", *((int *)v->ptr)); break;
    case CON_VAR_BOOL:
        if (*((bool *)v->ptr) == true)
            strcpy (val, "true");
        else
            strcpy (val, "false");
        break;
    default: val[0] = 0; break;
    }

    return val;
}
