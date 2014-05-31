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
static void CON_CmdClear(Console& console, const Console::Arguments& args)
{
    console.clear ();
}

/*
==================================================
Prikaz echo
==================================================
*/
static void CON_CmdEcho (Console& console, const Console::Arguments& args)
{
    int i, c = args.length();

    for (i = 1; i < c; i++)
        console.printf ("%s ", args.get(i).c_str());

    console.printf ("\n");
}

/*
==================================================
Prikaz list
==================================================
*/
static void CON_CmdList (Console& console, const Console::Arguments& args)
{
    const char *v1 = args.get(0).c_str();

    if (args.length() != 2)
    {
        console.printf (CON_Lang("CONSTR0006|%s : Pouziti %s <vars | cmds>\n"), v1, v1);
        return;
    }

    if (args.get(1) == "vars")
    {
        console.printf (CON_Lang("CONSTR0007|Registrovane promenne :\n"));

		if (console.listVars().empty())
		{
			console.printf(CON_Lang("CONSTR0008|\tNejsou registrovany zadne promenne\n"));
		}
		else
		{
			for (const Console::Variable& var : console.listVars())
			{
				console.printf("\t");
				var.printInfo(console);
			}
			console.printf (CON_Lang("CONSTR0009|\n\t%d promennych celkove\n"), console.listVars().size());
		}

        return;
    }

    if (args.get(1) == "cmds")
    {
        console.printf (CON_Lang("CONSTR0010|Registrovane prikazy :\n"));

		if (console.listCommands().empty())
		{
			console.printf(CON_Lang("CONSTR0011|\tZadne prikazy nebyly registrovany\n"));
		}
		else
		{
			for (const Console::Command& command : console.listCommands())
			{
				console.printf("\t\"%s\"\n", command.name.c_str());
			}
			console.printf(CON_Lang("CONSTR0012|\n\t%d prikazu celkem\n"), console.listCommands().size());
		}

        return;
    }

    console.printf (CON_Lang("CONSTR0013|%s : Pouziti %s <vars | cmds>\n"), v1, v1);
}

/*
==================================================
Prikaz dump
==================================================
*/
static void CON_CmdDump (Console& console, const Console::Arguments& args)
{
    FILE            *f;
    unsigned long   i, bufpos;
    bool            bufful;
    const conBYTE   *buf;
    const char      *v1 = args.get(0).c_str(),
                    *v2 = args.get(1).c_str();

    if (args.length() != 2)
    {
        console.printf (CON_Lang("CONSTR0014|%s : Pouziti %s <soubor>\n"), v1, v1);
        return;
    }

    f = fopen (v2, "wb");
    if (f == NULL)
    {
        console.printf (CON_Lang("CONSTR0015|%s : Nelze otevrit soubor %s\n"), v1, v2);
        return;
    }

    buf = console.getText(&bufpos, &bufful);
    i = bufful ? CON_TEXT_SIZE : bufpos;
    bufpos = bufful ? bufpos : 0;

    while (i-- > 0)
    {
        fwrite (&buf[bufpos], 1, 1, f);
        if (++bufpos >= CON_TEXT_SIZE)
            bufpos -= CON_TEXT_SIZE;
    }

    fclose (f);

    console.printf (CON_Lang("CONSTR0016|Obsah konzole ulozen do souboru %s\n"), v2);
}

/*
==================================================
Prikaz parse
==================================================
*/
static void CON_CmdParse (Console& console, const Console::Arguments& args)
{
    FILE    *f;
	const char *v1 = args.get(0).c_str(), *v2 = args.get(1).c_str();
	char o[100];

    if (args.length() != 2)
    {
        console.printf (CON_Lang("CONSTR0017|%s : Pouziti %s <soubor>\n"), v1, v1);
        return;
    }

    f = fopen (v2, "rb");
    if (f == nullptr)
    {
        console.printf (CON_Lang("CONSTR0018|%s : Nelze otevrit soubor %s\n"), v1, v2);
        return;
    }

    console.printf (CON_Lang("CONSTR0019|Provadeni souboru %s\n"), v2);
	std::string line;
	while (!feof(f))
	{
		char character;
		fread(&character, 1, 1, f);

		if (character == '\n')
		{
			console.appendCommands(line);
			line.clear();
		}
		else
		{
			line.push_back(character);
		}
	}
	console.appendCommands(line);

    sprintf (o, CON_Lang("CONSTR0020|echo Konec provadeni %s"), v2);
    console.prependCommands(o);
}

/*
==================================================
Prikaz alias
==================================================
*/
static void CON_CmdAlias (Console& console, const Console::Arguments& args)
{
    if (args.length() == 1)
    {
        console.printf (CON_Lang("CONSTR0021|Registrovane prikazy alias :\n"));

		if (console.listAliases().empty())
		{
			console.printf(CON_Lang("CONSTR0022|\tZadne\n"));
		}
		else
		{
			for (const Console::Alias& alias : console.listAliases())
			{
				console.printf("\t\"%s\" : %s\n", alias.name.c_str(), alias.command.c_str());
			}
		}
        return;
    }

	std::string command;
    for (size_t i = 2; i < args.length(); i++)
    {
        command.append(args.get(i));
		if (i != args.length() - 1)
		{
			command.push_back(' ');
		}
    }

    console.registerAlias(args.get(1), command);
}

/*
==================================================
Prikaz archive
==================================================
*/
static void CON_CmdArchive (Console& console, const Console::Arguments& args)
{
    int             i = 0;
    FILE            *f;
	const char *v1 = args.get(0).c_str();

    if (args.length() != 2)
    {
        console.printf (CON_Lang("CONSTR0023|%s : Pouziti %s <soubor>\n"), v1, v1);
        return;
    }

    f = fopen(args.get(1).c_str(), "wt");
    if (f == NULL)
    {
        console.printf (CON_Lang("CONSTR0024|%s : Nelze otevrit soubor\n"), args.get(1).c_str());
        return;
    }

    console.printf (CON_Lang("CONSTR0025|Archivuji :\n"));
    fprintf (f, "%s", CON_Lang("CONSTR0026|//Generovano prikazem archive\n//Nemente rucne\n\n"));
	for (const Console::Variable& var : console.listVars())
	{
		if (var.flags & CON_F_ARCHIVE)
		{
			i++;
			console.printf("\t\"%s\"\n", var.name.c_str());
			fprintf(f, "\"%s\" %s\n", var.name.c_str(), var.getValue().c_str());
		}
	}

    fclose (f);

    if (!i)
        console.printf (CON_Lang("CONSTR0027|\tZadna promenna nema archive flag\n"));
}

/*
==================================================
Registrace zakladnich prikazu
==================================================
*/
void CON_RegisterBasicCmd(Console& console)
{
    console.registerCommand(&CON_CmdClear, "clear");
    console.registerCommand(&CON_CmdEcho, "echo");
    console.registerCommand(&CON_CmdList, "list");
    console.registerCommand(&CON_CmdDump, "dump");
    console.registerCommand(&CON_CmdParse, "exec");
    console.registerCommand(&CON_CmdAlias, "alias");
    console.registerCommand(&CON_CmdArchive, "archive");
}
