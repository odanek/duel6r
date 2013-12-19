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
Projekt: Knihovna mylib
Popis: Zpracovani chybovych stavu
*/

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "mylib.h"

static char     myErrDumpString[MYERR_MAX_DUMP],
                myErrMsgString[MYERR_MAX_MSG];
static void     (*myErrProc) (const char *) = NULL;
static bool     myInError = false;

/*
==================================================
Zpracovani argumentu do jednoho retezce
==================================================
*/
char *MY_ErrDump (const char *s, ...)
{
    va_list argptr;

    va_start (argptr, s);
    vsprintf (myErrDumpString, s, argptr);
    va_end (argptr);

    return myErrDumpString;
}

/*
==================================================
Pripraveni chybove hlasky, volani chybove procedury
aplikace, ukonceni aplikace.
==================================================
*/
void MY_Error (const char *file, int line, const char *errStr)
{
    mySIZE  l;

    strcpy (myErrMsgString, MY_L("MYLIB0001|\n** CHYBA APLIKACE **\n\n"));
    strcat (myErrMsgString, MY_L("MYLIB0002|Chyba  : "));
    strcat (myErrMsgString, errStr);
    l = strlen (myErrMsgString);
    sprintf (&myErrMsgString[l], MY_L("MYLIB0003|\nSoubor : %s\nRadek  : %li\n"), file, line);
    fprintf (stderr, "%s", myErrMsgString);
    if (myErrProc != NULL && !myInError)
    {
        myInError = true;
        myErrProc (myErrMsgString);
    }

    exit (EXIT_FAILURE);
}

/*
==================================================
Nastaveni chybove procedury aplikace
==================================================
*/
void MY_ErrCallback (void (*errProc) (const char *))
{
    myErrProc = errProc;
}
