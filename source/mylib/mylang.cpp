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
Popis: Prace s jazykem, podpora ruznych run-time jazykovych verzi v aplikaci
*/

#include <map>
#include <string>
#include <string.h>
#include "mylib.h"

using namespace std;

static map<string,char*>        myLangMap;

/*
==================================================
Nahrani jednoho retezce
Retezce delsi nez MY_MAX_LANG_STR jsou oriznuty
Vraci true pri neuspechu
==================================================
*/
static bool MY_LangGetString (myFile_s *f, char *id, char *str)
{
    int     i;
    char    c;

    do
    {
        // Preskoci bile misto az k prvnimu znaku
        do
        {
            MY_FRead (&c, 1, 1, f);
            if (MY_FEof (f))
                return true;
        } while (c <= ' ');

        // Pokud je to komentar preskoci znaky az do konce radku
        if (c == '#')
        {
            do
            {
                MY_FRead (&c, 1, 1, f);
                if (MY_FEof (f))
                    return true;
            } while (c != '\n' && c != '\r');
        }
    } while (c == '\n' || c == '\r');

    // Nacti id - pokud prekroci povolenou delku je zkraceno (od konce!)
    i = 0;
    id[i++] = c;
    do
    {
        MY_FRead (&c, 1, 1, f);
        if (MY_FEof (f))
            return true;
        if (c > ' ' && i < MY_LANG_MAX_ID - 1)
            id[i++] = c;
    } while (c > ' ');
    id[i] = 0;

    // Nacti retezec - jsou od id oddeleny bud 1 mezerou nebo = nebo libovolnym poctem
    // tabulatoru a konci az s koncem radku. Jsou zkracovany jako idetifikatory.
    if (c == '\t')
        while (c == '\t')
        {
            MY_FRead (&c, 1, 1, f);
            if (MY_FEof (f))
                return true;
        }
    else
        if (c == ' ' || c == '=')
        {
            MY_FRead (&c, 1, 1, f);
            if (MY_FEof (f))
                return true;
        }
        else
            return true;

    i = 0;
    str[i++] = c;
    do
    {
        MY_FRead (&c, 1, 1, f);
        if (MY_FEof (f))
            break; // TODO: Otestovat nacitani posledniho radku
        if (c >= ' ' && i < MY_LANG_MAX_STR - 1)
            str[i++] = c;
    } while (c != '\n' && c!= '\r');

    str[i] = 0;
    return false;
}

/*
==================================================
Expandovani retezce - znaku \n, \r, \t, \v, \a, \b
Neumi expandovat oktalova a hex cisla
==================================================
*/
static void MY_LangExpandString (char *str)
{
    int     pos = 0, len = (int) strlen (str);
    char    nc;

    while (pos < len - 1)
    {
        if (str[pos] == '\\')
        {
            switch (str[pos + 1])
            {
            case 'n': nc = 10; break;
            case 't': nc = 9; break;
            case 'v': nc = 11; break;
            case 'b': nc = 8; break;
            case 'r': nc = 13; break;
            case 'a': nc = 7; break;
            default : nc = str[pos + 1];
            }
            memmove (&str[pos], &str[pos + 1], len - pos);
            len--;
            str[pos] = nc;
        }
        pos++;
    }
}

/*
==================================================
Uvolneni pameti obsazene aktualnim jazykem
==================================================
*/
void MY_LangFree (void)
{
    map<string,char*>::iterator     m_it;

    for (m_it = myLangMap.begin(); m_it != myLangMap.end(); m_it++)
        MY_Free (m_it->second);
    myLangMap.clear();
}

/*
==================================================
Nahrani retezcu - jazyka
Pokud se jazyk nepovede nahrat je ponechan
puvodni jazyk (cestina)
==================================================
*/
bool MY_LangLoad (const char *lang_file)
{
    myFile_s    *f;
    char        id[MY_LANG_MAX_ID], str[MY_LANG_MAX_STR];
    string      id_str;
    bool        ok = true;

    f = MY_FOpen (lang_file, 0, "rb", false);
    if (f == NULL)
        return true;

    MY_LangFree ();

    while (ok)
    {
        ok = !MY_LangGetString (f, id, str);
        if (ok)
        {
            MY_LangExpandString (str);
            id_str = id;
            if (myLangMap.find (id_str) == myLangMap.end())
                myLangMap.insert (pair<string,char*> (id_str, MY_StrDup (str)));
        }
    }

    MY_FClose (&f);
    return false;
}

/*
==================================================
Vraci zadany retezec v nahranem jazyce, pokud byl
nejaky nahran, v opacnem pripade vraci predany retezec.
==================================================
*/
const char *MY_L (const char *text)
{
    int                             i = 0, j = 0;
    map<string,char*>::iterator     m_it;
    string                          id;

    // Nalezni oddelovac |
    while (text[i] != '|' && text[i] != 0 && text[i] != ' ')
        i++;

    // Pokud nebyl nalezen oddelovac identifikatoru nebo je id chybny
    if (text[i] != '|')
        return text;

    if (!myLangMap.empty())
    {
        while (j < i)
            id += text[j++];
        m_it = myLangMap.find (id);
        if (m_it != myLangMap.end())
            return m_it->second;
    }

    return &text[i + 1];
}
