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

#include <sstream>
#include <stdlib.h>
#include <string.h>
#include "console.h"

/*
==================================================
Zaregistrovani promene
==================================================
*/
int con_c::regvar (void *p, const std::string& name, int flags, conVarType_e typ)
{
    int i = namevalid (name, CON_Lang("CONSTR0039|Regitrace promenne"), p);
    if (i != CON_SUCCES)
        return i;

	conVar_s newVar;
	newVar.name = name;
    newVar.ptr = p;
    newVar.flags = flags;
    newVar.type = typ;

    // Zaradi novou promenou tak, aby byly setridene podle abecedy
	size_t position = 0;
	for (const conVar_s& var : m_vars)
	{
		if (var.name.compare(newVar.name) > 0)
		{
			break;
		}
		++position;
	}

	m_vars.insert(m_vars.begin() + position, newVar);

    if (m_flags & CON_F_REG_INFO)
        printf (CON_Lang("CONSTR0040|Registrace promenne: \"%s\" na adrese 0x%p byla uspesna\n"), name.c_str(), p);

    return CON_SUCCES;
}

conVar_s *con_c::findVar(const std::string& name)
{
	for (conVar_s& var : m_vars)
	{
		if (var.name == name)
		{
			return &var;
		}
	}

    return nullptr;
}

void con_c::varCmd(conVar_s& var)
{
    int c = argc ();

    if (c > 2)
    {
        printf (CON_Lang("CONSTR0041|Promenne : Pouziti jmeno_promenne [nova_hodnota]\n"));
        return;
    }

	if (c == 1)
	{
		varInfo(var);
	}
	else
	{
		if (!(var.flags & CON_F_RONLY))
			setVarValue(var, argv(1));
		else
			printf(CON_Lang("CONSTR0042|Promenna \"%s\" je pouze pro cteni\n"), var.name.c_str());
	}
}

/*
==================================================
Vytiskne na konzolu info o promene
==================================================
*/
void con_c::varInfo(const conVar_s& var)
{
    const char  flagstr[CON_FLAGS + 1] = "ra",
                *typestr[3] = { "float", "int", "bool" };
    int         i, f = 1;

	for (i = 0; i < CON_FLAGS; i++, f <<= 1)
	{
		if (var.flags & f)
			printf("%c", flagstr[i]);
		else
			printf("-");
	}

    printf (CON_Lang("CONSTR0043| %-5s \"%s\" s hodnotou %s\n"), typestr[var.type], var.name.c_str(), getVarValue(var).c_str());
}

/*
==================================================
Nastavi hodnotu promene podle retezce val
==================================================
*/
void con_c::setVarValue(conVar_s& var, const std::string& val)
{
    switch (var.type)
    {
    case CON_VAR_FLOAT: *((float *)var.ptr) = (float) atof(val.c_str()); break;
    case CON_VAR_INT: *((int *)var.ptr) = atoi(val.c_str()); break;
    case CON_VAR_BOOL: *((bool *)var.ptr) = (val == "true"); break;
    }
}

/*
==================================================
Ulozi hodnotu promene do retezce val
==================================================
*/
std::string con_c::getVarValue(const conVar_s& var) const
{
	if (var.type == CON_VAR_FLOAT)
	{
		std::ostringstream stream;
		float *val = (float *)var.ptr;
		stream << *val;
		return stream.str();
	}
	else if (var.type == CON_VAR_INT)
	{
		std::ostringstream stream;
		int *val = (int *)var.ptr;
		stream << *val;
		return stream.str();
	}
	else if (var.type == CON_VAR_BOOL)
	{
		return (*((bool *)var.ptr) == true) ? "true" : "false";
	}

	return "";
}
