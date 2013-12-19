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
Popis: Hlavni routiny - prace s pameti
*/

#include <malloc.h>
#include <string.h>
#include "mylib.h"

#if defined (_MSC_VER) || defined (MY_LEAK_CHECK)
static mySIZE       myMemUsed = 0;
#endif

#ifdef MY_LEAK_CHECK
    #include <string>
    #include <vector>

    struct myLeak_s
    {
        std::string file;
        int         line;
        mySIZE      size;
        void        *ptr;
    };
    std::vector<myLeak_s *> myMemLeak;
#endif

/*
==================================================
Vraci mnozstvi alokovane pameti
Pouze pro Visual studio nebo s MY_LEAK_CHECK
==================================================
*/
#if defined (_MSC_VER) || defined (MY_LEAK_CHECK)
mySIZE MY_MemUsed (void)
{
    return myMemUsed;
}
#endif

/*
==================================================
Do seznamu alokovane pameti prida novy blok
==================================================
*/
void MY_RegMem_internal (void *ptr, mySIZE sz, const char *fl, int line)
{
#ifdef MY_LEAK_CHECK
    myLeak_s    *ml;

    ml = new myLeak_s;
    ml->file = fl;
    ml->line = line;
    ml->size = sz;
    ml->ptr = ptr;
    myMemLeak.push_back(ml);
    myMemUsed += sz;
#elif defined (_MSC_VER)
    myMemUsed += sz;
#endif
}

/*
==================================================
Odebere blok ze seznamu alokovane pameti
==================================================
*/
void MY_UnregMem_internal (void *ptr)
{
#ifdef MY_LEAK_CHECK
    std::vector<myLeak_s *>::iterator   i;
    for (i = myMemLeak.begin(); i != myMemLeak.end(); i++)
        if ((*i)->ptr == ptr)
        {
            myMemUsed -= (*i)->size;
            delete *i;
            myMemLeak.erase(i);
            break;
        }
#elif defined (_MSC_VER)
    myMemUsed -= _msize (ptr);
#endif
}

/*
==================================================
Alokuje blok pameti o dane velikosti
==================================================
*/
void *MY_Alloc_internal (const char *file, int line, mySIZE size)
{
    void    *p;

    if (size < 1)
        MY_Error (file, line, MY_L("MYLIB0007|Pokus o alokovani nula bytu"));

    p = malloc (size);
    if (p == NULL)
        MY_Error (file, line, MY_L("MYLIB0008|Nedostatek volne pameti"));

    MY_RegMem_internal (p, size, file, line);

    return p;
}

/*
==================================================
Alokuje blok pameti o dane velikosti a naplni jej
nulami
==================================================
*/
void *MY_CAlloc_internal (const char *file, int line, mySIZE size)
{
    void    *p;

    p = MY_Alloc_internal (file, line, size);
    memset (p, 0, size);
    return p;
}

/*
==================================================
Uvolni blok pameti
==================================================
*/
void MY_FreeMem (void *p)
{
    if (p != NULL)
    {
        MY_UnregMem_internal (p);
        free (p);
    }
}

/*
==================================================
Vypise neuvolnenou pamet
Pouzitelne pouze pod windows
==================================================
*/
#ifdef MY_LEAK_CHECK
void MY_ShowLeaks (FILE *f)
{
    for (std::vector<myLeak_s *>::iterator i = myMemLeak.begin(); i != myMemLeak.end(); i++)
        fprintf (f, "WARNING: Leak %d bytes in %s at %d\n", (*i)->size, (*i)->file.c_str(), (*i)->line);
}
#endif

/*
==================================================
Vyplni blok pameti danym slovem
==================================================
*/
void MY_MemSetW (myWORD *p, myWORD fillWith, mySIZE size)
{
    while (size-- > 0)
        *(p++) = fillWith;
}

/*
==================================================
Vytvori kopii retezce a vrati ukazatel
==================================================
*/
char *MY_StrDup_internal (const char *file, int line, const char *str)
{
    if (str == NULL)
        return NULL;
    return strcpy ((char *) MY_Alloc_internal (file, line, strlen (str) + 1), str);
}
