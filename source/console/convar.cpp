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
int Console::registerVariable(void *p, const std::string& name, int flags, Variable::Type type)
{
    int i = isNameValid(name, CON_Lang("Variable registration"), p);
    if (i != CON_SUCCES)
        return i;

	Variable newVar;
	newVar.name = name;
    newVar.ptr = p;
    newVar.flags = flags;
    newVar.type = type;

    // Zaradi novou promenou tak, aby byly setridene podle abecedy
	size_t position = 0;
	for (const Variable& var : vars)
	{
		if (var.name.compare(newVar.name) > 0)
		{
			break;
		}
		++position;
	}

	vars.insert(vars.begin() + position, newVar);

	if (flags & CON_F_REG_INFO)
	{
		print(CON_Format(CON_Lang("Variable registration: \"{0}\" at address {1} has been successful\n")) << name << p);
	}

    return CON_SUCCES;
}

Console::Variable *Console::findVar(const std::string& name)
{
	for (Variable& var : vars)
	{
		if (var.name == name)
		{
			return &var;
		}
	}

    return nullptr;
}

void Console::varCmd(Variable& var, Arguments& args)
{
    size_t c = args.length();

    if (c > 2)
    {
        print(CON_Lang("CONSTR0041|Variables : Usage variable_name [new_value]\n"));
        return;
    }

	if (c == 1)
	{
		var.printInfo(*this);
	}
	else
	{
		if (!(var.flags & CON_F_RONLY))
		{
			var.setValue(args.get(1));
		}
		else
		{
			print(CON_Format(CON_Lang("Variable \"{0}\" is read-only\n")) << var.name);
		}
	}
}

/*
==================================================
Vytiskne na konzolu info o promene
==================================================
*/
void Console::Variable::printInfo(Console& console) const
{
    const char  flagstr[CON_FLAGS + 1] = "ra",
                *typestr[3] = { "float", "int", "bool" };
    int         i, f = 1;

	for (i = 0; i < CON_FLAGS; i++, f <<= 1)
	{
		if (flags & f)
		{
			console.print(std::string(1, flagstr[i]));
		}
		else
		{
			console.print("-");
		}
	}

	console.print(CON_Format(CON_Lang(" {0} \"{1}\" with value {2}\n")) << typestr[(int)type] << name << getValue());
}

/*
==================================================
Nastavi hodnotu promene podle retezce val
==================================================
*/
void Console::Variable::setValue(const std::string& val)
{
    switch (type)
    {
	case Type::Float: *((float *)ptr) = std::stof(val); break;
	case Type::Int: *((int *)ptr) = std::stoi(val); break;
	case Type::Bool: *((bool *)ptr) = (val == "true"); break;
    }
}

/*
==================================================
Ulozi hodnotu promene do retezce val
==================================================
*/
std::string Console::Variable::getValue() const
{
	if (type == Type::Float)
	{
		std::ostringstream stream;
		float *val = (float *)ptr;
		stream << *val;
		return stream.str();
	}
	else if (type == Type::Int)
	{
		std::ostringstream stream;
		int *val = (int *)ptr;
		stream << *val;
		return stream.str();
	}
	else if (type == Type::Bool)
	{
		return (*((bool *)ptr) == true) ? "true" : "false";
	}

	return "";
}
