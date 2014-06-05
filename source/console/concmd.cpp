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
Clear command
==================================================
*/
static void CON_CmdClear(Console& console, const Console::Arguments& args)
{
    console.clear();
}

/*
==================================================
Echo command
==================================================
*/
static void CON_CmdEcho(Console& console, const Console::Arguments& args)
{
	for (size_t i = 1; i < args.length(); i++)
	{
		console.print(CON_Format("{0} ") << args.get(i));
	}

    console.print("\n");
}

/*
==================================================
List command
==================================================
*/
static void CON_CmdList(Console& console, const Console::Arguments& args)
{
    if (args.length() != 2)
    {
		console.print(CON_Format(CON_Lang("{0} : Usage {0} <vars | cmds>\n")) << args.get(0));
        return;
    }

    if (args.get(1) == "vars")
    {
        console.print(CON_Lang("Registered variables:\n"));

		if (console.listVars().empty())
		{
			console.print(CON_Lang("There are zero registered variables\n"));
		}
		else
		{
			for (const Console::Variable& var : console.listVars())
			{
				console.print("\t");
				var.printInfo(console);
			}
			console.print(CON_Format(CON_Lang("\n\t{0} variables in total\n")) << console.listVars().size());
		}

        return;
    }

    if (args.get(1) == "cmds")
    {
        console.print(CON_Lang("Registered commands:\n"));

		if (console.listCommands().empty())
		{
			console.print(CON_Lang("There are zero registered commands\n"));
		}
		else
		{
			for (const Console::Command& command : console.listCommands())
			{
				console.print(CON_Format("\t\"{0}\"\n") << command.name);
			}
			console.print(CON_Format(CON_Lang("\n\t{0} commands in total\n")) << console.listCommands().size());
		}

        return;
    }

	console.print(CON_Format(CON_Lang("{0} : Usage {0} <vars | cmds>\n")) << args.get(0));
}

/*
==================================================
Dump command
==================================================
*/
static void CON_CmdDump(Console& console, const Console::Arguments& args)
{
    FILE            *f;
    unsigned long   i, bufpos;
    bool            bufful;
    const conBYTE   *buf;

    if (args.length() != 2)
    {
		console.print(CON_Format(CON_Lang("{0} : Usage {0} <file_name>\n")) << args.get(0));
        return;
    }

    f = fopen(args.get(1).c_str(), "wb");
    if (f == nullptr)
    {
		console.print(CON_Format(CON_Lang("{0} : Unable to open file {1}\n")) << args.get(0) << args.get(1));
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

	console.print(CON_Format(CON_Lang("Console content has been saved to file {0}\n")) << args.get(1));
}

/*
==================================================
Parse command
==================================================
*/
static void CON_CmdParse(Console& console, const Console::Arguments& args)
{
    FILE    *f;

    if (args.length() != 2)
    {
		console.print(CON_Format(CON_Lang("{0} : Usage {0} <file_name>\n")) << args.get(0));
        return;
    }

    f = fopen(args.get(1).c_str(), "rb");
    if (f == nullptr)
    {
		console.print(CON_Format(CON_Lang("{0} : Unable to open file {1}\n")) << args.get(0) << args.get(1));
        return;
    }

	console.print(CON_Format(CON_Lang("Executing file: {0}\n")) << args.get(1));
	std::string content;
	while (!feof(f))
	{
		char character;
		fread(&character, 1, 1, f);
		content.push_back(character);
	}

	console.prependCommands(CON_Format(CON_Lang("echo Finished execution of {0}")) << args.get(1));
	console.prependCommands(content);
}

/*
==================================================
Alias command
==================================================
*/
static void CON_CmdAlias(Console& console, const Console::Arguments& args)
{
    if (args.length() == 1)
    {
        console.print(CON_Lang("Registered aliases:\n"));

		if (console.listAliases().empty())
		{
			console.print(CON_Lang("\tNone\n"));
		}
		else
		{
			for (const Console::Alias& alias : console.listAliases())
			{
				console.print(CON_Format("\t\"{0}\" : {1}\n") << alias.name << alias.command);
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
Archive command
==================================================
*/
static void CON_CmdArchive(Console& console, const Console::Arguments& args)
{
    int             i = 0;
    FILE            *f;

    if (args.length() != 2)
    {
		console.print(CON_Format(CON_Lang("{0} : Usage {0} <file_name>\n")) << args.get(0));
        return;
    }

    f = fopen(args.get(1).c_str(), "wt");
    if (f == NULL)
    {
		console.print(CON_Format(CON_Lang("{0} : Unable to open file {1}\n")) << args.get(0) << args.get(1));
        return;
    }

    console.print(CON_Lang("Archivinig :\n"));
    fprintf (f, "%s", CON_Lang("//Generated by archive command\n//Do not edit by hand\n\n").c_str());
	for (const Console::Variable& var : console.listVars())
	{
		if (var.flags & CON_F_ARCHIVE)
		{
			i++;
			console.print(CON_Format("\t\"{0}\"\n") << var.name);
			fprintf(f, "\"%s\" %s\n", var.name.c_str(), var.getValue().c_str());
		}
	}

    fclose (f);

	if (!i)
	{
		console.print(CON_Lang("\tThere are no variables with archive flag\n"));
	}
}

/*
==================================================
Register basic console commands
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
