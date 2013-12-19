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
Popis: Prace se soubory a s knihovnami souboru
*/

#include <string.h>
#include "mylib.h"

// Pouzivane struktury
struct myPckfile_s
{
    char    *name;
    myDWORD offs;
    myDWORD size;
};

struct myPcklib_s
{
    char                *lib_name;
    int                 num_files;
    myPckfile_s         *pck_file;
    struct myPcklib_s   *pck_next;
};

static myPcklib_s   *myPckLib = NULL;
static myFile_s     myFile[MYFILE_MAX_OPENED];

/*
==================================================
Inicializace knihovny
==================================================
*/
void MY_Init (void)
{
    int     i;

    // Inicializace souboru
    for (i = 0; i < MYFILE_MAX_OPENED; i++)
        myFile[i].opened = false;
}

/*
==================================================
Deinicializace knihovny
==================================================
*/
void MY_DeInit (void)
{
    MY_FreePck ();
    MY_LangFree ();

#ifdef MY_LEAK_CHECK
    // Vypise na chybovy vystup neuvolnenou pamet
    if (MY_MemUsed ())
        MY_ShowLeaks (stderr);
#endif
}

/*
==================================================
Zkontroluje zda uz nebyla knihovna souboru
nactena.
==================================================
*/
static int MY_CheckIfAdded (const char *fr)
{
    myPcklib_s  *plib = myPckLib;

    while (plib != NULL)
    {
        if (!strcmp (plib->lib_name, fr))
            return 1;
        plib = plib->pck_next;
    }
    return 0;
}

/*
==================================================
Prida knihovnu souboru
==================================================
*/
int MY_AddPck (const char *fr)
{
    myPcklib_s  *plib;
    FILE        *f;
    long        header[2];
    size_t      l;
    int         i, len = 0;

    if (MY_CheckIfAdded (fr))
        return MY_ALREADY;

    f = fopen (fr, "rb");
    if (f == NULL)
        return MY_FAILED;

    l = fread (header, 4, 2, f);
    if ((header[0] != MYFILE_PCK1_ID && header[0] != MYFILE_PCK2_ID) || header[1] < 1 || l != 2)
    {
        fclose (f);
        return MY_FAILED;
    }

    plib            = MY_MALLOC (myPcklib_s, 1);
    plib->num_files = header[1];
    plib->lib_name  = MY_StrDup (fr);
    plib->pck_next  = myPckLib;
    plib->pck_file  = MY_MALLOC (myPckfile_s, plib->num_files);

    for (i = 0; i < plib->num_files; i++)
    {
        if (header[0] == MYFILE_PCK1_ID)
            len = MYFILE_PCK1_MAX_NAME;
        if (header[0] == MYFILE_PCK2_ID)
            if (fread ((void *)&len, 4, 1, f) != 1)
                return MY_FAILED;

        plib->pck_file[i].name = (char *) MY_Alloc (len);

        if (fread ((void *) plib->pck_file[i].name, len, 1, f) != 1)
            return MY_FAILED;
        if (fread ((void *) &plib->pck_file[i].offs, 4, 1, f) != 1)
            return MY_FAILED;
        if (fread ((void *) &plib->pck_file[i].size, 4, 1, f) != 1)
            return MY_FAILED;
    }

    fclose (f);

    myPckLib = plib;
    return MY_OK;
}

/*
==================================================
Uvolni vsechny knihovny z pameti
==================================================
*/
void MY_FreePck (void)
{
    int         i;
    myPcklib_s  *plib, *next;

    plib = myPckLib;
    while (plib != NULL)
    {
        for (i = 0; i < plib->num_files; i++)
            MY_Free (plib->pck_file[i].name);
        MY_Free (plib->lib_name);
        MY_Free (plib->pck_file);
        next = plib->pck_next;
        MY_Free (plib);
        plib = next;
    }

    myPckLib = NULL;
}

/*
==================================================
Zjisti velikost souboru (transparentne vzhledem
ke knihovnam)
==================================================
*/
mySIZE MY_FSize (const char *fr)
{
    myFile_s    *f;
    long        sz;

    f = MY_FOpen (fr, 0, "rb", false);
    if (f == NULL)
        return 0;

    MY_FSeek (f, 0, SEEK_END);
    sz = MY_FTell (f);
    MY_FClose (&f);
    return sz;
}

/*
==================================================
Presune se na danou pozici v souboru
==================================================
*/
void MY_FSeek (myFile_s *f, long pos, int style)
{
    if (f->size == -1)
        fseek (f->file, pos, style);
    else
    {
        if (style == SEEK_SET)
            fseek (f->file, f->offs + pos, SEEK_SET);
        else if (style == SEEK_CUR)
            fseek (f->file, pos, SEEK_CUR);
        else if (style == SEEK_END)
            fseek (f->file, f->offs + f->size + pos, SEEK_SET);
    }
}

/*
==================================================
Otevre soubor
==================================================
*/
myFile_s *MY_FOpen (const char *fr, long ofst, const char *mod, bool must)
{
    myPcklib_s *plib;
    myFile_s    *f = NULL;
    int         i;

    for (i = 0; i < MYFILE_MAX_OPENED; i++)
        if (!myFile[i].opened)
        {
            f = &myFile[i];
            break;
        }

    if (f == NULL)
    {
        if (must)
            MY_Err (MY_L("MYLIB0004|Prilis mnoho otevrenych souboru"));
        return NULL;
    }

    f->file = fopen (fr, mod);
    if (f->file == NULL)
    {
        plib = myPckLib;
        while (plib != NULL)
        {
            for (i = 0; i < plib->num_files; i++)
                if (!strcmp (fr, plib->pck_file[i].name))
                {
                    f->offs = plib->pck_file[i].offs;
                    f->file = fopen (plib->lib_name, mod);
                    if (f->file == NULL)
                    {
                        if (must)
                            MY_Err (MY_ErrDump (MY_L("MYLIB0005|Nelze otevrit soubor %s"), fr));
                        return NULL;
                    }
                    f->size = plib->pck_file[i].size;
                    f->opened = true;
                    fseek (f->file, f->offs + ofst, SEEK_SET);
                    return f;
                }
            plib = plib->pck_next;
        }

        if (must)
            MY_Err (MY_ErrDump (MY_L("MYLIB0006|Nelze otevrit soubor %s"), fr));
        return NULL;
    }

    f->offs = 0;
    f->size = -1;
    f->opened = true;
    fseek (f->file, ofst, SEEK_SET);
    return f;
}

/*
==================================================
Nacte obsah souboru od urcite pozice s
danou velikosti do pameti.
Pokud je sz = -1 pak az do konce souboru
==================================================
*/
void MY_FLoadBlock (const char *fl, long fr, long sz, void *data)
{
    myFile_s    *f;

    if (sz == -1)
        sz = (long) MY_FSize (fl) - fr;

    f = MY_FOpen (fl, fr, "rb", true);
    MY_FRead (data, 1, sz, f);
    MY_FClose (&f);
}

/*
==================================================
Uzavreni souboru
==================================================
*/
int MY_FClose (myFile_s **f)
{
    int ret_val = 0;

    if (*f != NULL)
    {
        if ((*f)->file != NULL)
            ret_val = fclose ((*f)->file);
        (*f)->file = NULL;
        (*f)->opened = false;
    }
    *f = NULL;
    return ret_val;
}

/*
==================================================
Zjisteni aktualni pozice v souboru
==================================================
*/
long MY_FTell (myFile_s *f)
{
    return ftell (f->file) - f->offs;
}

/*
==================================================
Zjisti zda uz byl cten konec souboru (pouze binarni)
==================================================
*/
bool MY_FEof (myFile_s *f)
{
    if (f->size == -1)
        return (feof (f->file) != 0);
    return (MY_FTell (f) > f->size || feof (f->file));
}
