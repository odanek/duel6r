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
Popis: Prace s knihovnami obrazku
*/

#include <string.h>
#include "mylib.h"

static myFile_s     *myKh3File = NULL;
static myKh3info_s  myKh3Info;

/*
==================================================
Otevreni knihovny
==================================================
*/
int MY_KH3Open (const char *fl)
{
    myDWORD     t;

    if (myKh3File != NULL)
        return MY_ALREADY;

    if (MY_FSize (fl) < sizeof (myKh3info_s))
        return MY_FAILED;

    myKh3File = MY_FOpen (fl, 0, "rb", false);
    if (myKh3File == NULL)
        return 1;

    // Musi byt po slozkach jinak by se musel menit pack u struktur => nizsi prenositelnost
    MY_FRead (&myKh3Info.id, 20, 1, myKh3File);
    MY_FRead (&myKh3Info.sizex, 4, 1, myKh3File);
    MY_FRead (&myKh3Info.sizey, 4, 1, myKh3File);
    MY_FRead (&myKh3Info.type, 4, 1, myKh3File);
    MY_FRead (&myKh3Info.picts, 4, 1, myKh3File);

    if (strcmp (myKh3Info.id, MY_KH3_ID))
    {
        MY_FClose (&myKh3File);
        return MY_FAILED;
    }

    t = myKh3Info.type;
    if (t != MY_KH3_TYP_OB1 && t != MY_KH3_TYP_OB3 && t != MY_KH3_TYP_OB5 && t != MY_KH3_TYP_OB7)
    {
        MY_FClose (&myKh3File);
        return MY_FAILED;
    }

    return MY_OK;
}

/*
==================================================
Zavreni knihovny
==================================================
*/
void MY_KH3Close (void)
{
    MY_FClose (&myKh3File);
}

/*
==================================================
Vraci pocet obazku v knihovne
==================================================
*/
int MY_KH3Images (void)
{
    if (myKh3File == NULL)
        return 0;

    return myKh3Info.picts;
}

/*
==================================================
Rutiny pro nacitani jednotlivych typu
==================================================
*/
static void KH3_LoadOb3 (myFile_s *f, myDWORD size, myWORD *data, int l)
{
    myDWORD i = 0;
    myWORD  j, dl = 0, cl;

    while (i < size)
    {
        MY_FRead (&dl, l, 1, f);
        MY_FRead (&cl, 2, 1, f);
        for (j = 0; j < dl; j++)
            data[i + j] = cl;
        i += dl;
    }
}

static void KH3_LoadOb7 (myFile_s *f, myDWORD size, myWORD *data)
{
    myBYTE  head;
    myWORD  col;
    myDWORD i = 0, sz;

    while (i < size)
    {
        sz = 0;
        MY_FRead (&head, 1, 1, f);

        if (head == MY_KH3_OB7_RAW_BYTE)
        {
            MY_FRead (&sz, 1, 1, f);
            MY_FRead (&data[i], 2, sz, f);
        }
        else if (head == MY_KH3_OB7_RAW_WORD)
        {
            MY_FRead (&sz, 2, 1, f);
            MY_FRead (&data[i], 2, sz, f);
        }
        else if (head == MY_KH3_OB7_RLE_BYTE)
        {
            MY_FRead (&sz, 1, 1, f);
            MY_FRead (&col, 2, 1, f);
            MY_MemSetW (&data[i], col, sz);
        }
        else if (head == MY_KH3_OB7_RLE_WORD)
        {
            MY_FRead (&sz, 2, 1, f);
            MY_FRead (&col, 2, 1, f);
            MY_MemSetW (&data[i], col, sz);
        }
        else if (head > MY_KH3_OB7_RLE)
        {
            sz = head - MY_KH3_OB7_RLE;
            MY_FRead (&col, 2, 1, f);
            MY_MemSetW (&data[i], col, sz);
        }
        else
        {
            sz = head;
            MY_FRead (&data[i], 2, sz, f);
        }

        i += sz;
    }
}

/*
==================================================
Nacte obrazek do pameti
==================================================
*/
void MY_KH3Load (myWORD n, myWORD *data)
{
    myDWORD pos;

    if (myKh3File == NULL || n >= myKh3Info.picts)
        return;

    MY_FSeek (myKh3File, 36 + (n << 2), SEEK_SET);
    MY_FRead (&pos, 4, 1, myKh3File);
    MY_FSeek (myKh3File, pos, SEEK_SET);
    pos = myKh3Info.sizex * myKh3Info.sizey;

    switch (myKh3Info.type)
    {
        case MY_KH3_TYP_OB1:
            MY_FRead (data, 2, pos, myKh3File);
            break;
        case MY_KH3_TYP_OB3:
            KH3_LoadOb3 (myKh3File, pos, data, 2);
            break;
        case MY_KH3_TYP_OB5:
            KH3_LoadOb3 (myKh3File, pos, data, 1);
            break;
        case MY_KH3_TYP_OB7:
            KH3_LoadOb7 (myKh3File, pos, data);
            break;
    }
}

/*
==================================================
Vraci info o knihovne
==================================================
*/
int MY_KH3GetInfo (myKh3info_s *info)
{
    if (myKh3File != NULL)
    {
        memcpy (info, &myKh3Info, sizeof (myKh3info_s));
        return MY_OK;
    }
    return MY_FAILED;
}
