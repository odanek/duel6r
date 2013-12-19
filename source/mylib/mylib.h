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
Popis: Hlavni hlavickovy soubor
*/

#ifndef __MYLIB_H
#define __MYLIB_H

#include <stdio.h>

#define MYLIB_VERSION   "3.2.5"

typedef unsigned char   myBYTE;
typedef unsigned short  myWORD;
typedef unsigned long   myDWORD;
typedef unsigned int    myUINT;
typedef size_t          mySIZE;

#define MY_Max(x,y)     ((x) > (y) ? (x) : (y))
#define MY_Min(x,y)     ((x) > (y) ? (y) : (x))
#define MY_MALLOC(t,s)  ((t *) MY_Alloc (s * sizeof (t)))

// Assertions
#if defined(DEBUG) || defined (_DEBUG)
    #define MY_ASSERT(x,y)  if (!(x)) MY_Err (y);
#else
    #define MY_ASSERT(x,y)
#endif

// Leak check
#if defined(_DEBUG)
    #define MY_LEAK_CHECK
#endif

/////////////////////////////////////////////////////////////////////
//                          Knihovna
/////////////////////////////////////////////////////////////////////
#define MY_OK                   0
#define MY_FAILED               1
#define MY_ALREADY              2

void    MY_Init                 (void);
void    MY_DeInit               (void);

/////////////////////////////////////////////////////////////////////
//                          MYerror.cpp
/////////////////////////////////////////////////////////////////////
#define MYERR_MAX_DUMP          1024
#define MYERR_MAX_MSG           1024

void    MY_Error                (const char *file, int line, const char *errStr);
char    *MY_ErrDump             (const char *s, ...);
void    MY_ErrCallback          (void (*errProc) (const char *));

#define MY_Err(x)               MY_Error (__FILE__, __LINE__, x)
#define MY_ErrTest(x,y)         if (x) MY_Error (__FILE__, __LINE__, y)

/////////////////////////////////////////////////////////////////////
//                          MYfile.cpp
/////////////////////////////////////////////////////////////////////
#define MYFILE_PCK1_ID          0x014B4350L
#define MYFILE_PCK2_ID          0x024B4350L
#define MYFILE_PCK1_MAX_NAME    52
#define MYFILE_MAX_OPENED       15

struct myFile_s
{
    long    offs;
    long    size;
    FILE    *file;
    bool    opened;
};

int     MY_AddPck           (const char *fr);
void    MY_FreePck          (void);
myFile_s *MY_FOpen          (const char *fl, long ofst, const char *mod, bool must);
mySIZE  MY_FSize            (const char *fl);
void    MY_FSeek            (myFile_s *f, long pos, int style);
size_t  MY_FRead            (void *buffer, size_t size, size_t count, myFile_s *f);
size_t  MY_FWrite           (const void *buffer, size_t size, size_t count, myFile_s *f);
long    MY_FTell            (myFile_s *f);
bool    MY_FEof             (myFile_s *f);
int     MY_FClose           (myFile_s **f);
void    MY_FLoadBlock       (const char *fl, long fr, long sz, void *data);

/////////////////////////////////////////////////////////////////////
//                      MYtools.cpp
/////////////////////////////////////////////////////////////////////
void    *MY_Alloc_internal  (const char *file, int line, mySIZE size);
void    *MY_CAlloc_internal (const char *file, int line, mySIZE size);
char    *MY_StrDup_internal (const char *file, int line, const char *str);
void    MY_RegMem_internal   (void *ptr, mySIZE sz, const char *fl, int line);
void    MY_UnregMem_internal (void *ptr);
void    MY_MemSetW          (myWORD *p, myWORD fillWith, mySIZE size);

// Allocating memory
#define MY_Alloc(x)         MY_Alloc_internal (__FILE__, __LINE__, x)
#define MY_CAlloc(x)        MY_CAlloc_internal (__FILE__, __LINE__, x)
#define MY_StrDup(x)        MY_StrDup_internal (__FILE__, __LINE__, x)
#define MY_RegMem(x,s)      MY_RegMem_internal ((void *)x, s, __FILE__, __LINE__)
#define MY_UnregMem(x)      MY_UnregMem_internal ((void *)x)

// Freeing memory - done in this way in order not to break
// strict aliasing rules (in GCC)
void    MY_FreeMem          (void *p);
#define MY_Free(x)          MY_FreeMemT (&(x))
template<class T>
    inline void MY_FreeMemT (T **p)
    {
        MY_FreeMem ((void *) *p);
        *p = NULL;
    }

// Leak checking
#if defined(_MSC_VER) || defined (MY_LEAK_CHECK)
mySIZE  MY_MemUsed          (void);
#endif

#ifdef MY_LEAK_CHECK
void    MY_ShowLeaks        (FILE *f);
#endif

/////////////////////////////////////////////////////////////////////
//                       MYkh3.cpp
/////////////////////////////////////////////////////////////////////

#define MY_KH3_ID               "Kresli 3 Hi-Color  "
#define MY_KH3_OB7_RAW_BYTE     0x00
#define MY_KH3_OB7_RAW_WORD     0xFF
#define MY_KH3_OB7_RLE_BYTE     0x80
#define MY_KH3_OB7_RLE_WORD     0x81
#define MY_KH3_OB7_RLE          0x80

enum
{
    MY_KH3_TYP_OB1 = 1,
    MY_KH3_TYP_OB3 = 2,
    MY_KH3_TYP_OB5 = 4,
    MY_KH3_TYP_OB7 = 7
};

struct myKh3info_s
{
    char    id[20];
    myDWORD sizex;
    myDWORD sizey;
    myDWORD type;
    myDWORD picts;
};

int     MY_KH3Open      (const char *fl);
void    MY_KH3Close     (void);
int     MY_KH3Images    (void);
void    MY_KH3Load      (myWORD n, myWORD *data);
int     MY_KH3GetInfo   (myKh3info_s *info);

/////////////////////////////////////////////////////////////////////
//                       MYlang.cpp
/////////////////////////////////////////////////////////////////////
#define MY_LANG_MAX_ID      20
#define MY_LANG_MAX_STR     500

bool        MY_LangLoad     (const char *lang_file);
void        MY_LangFree     (void);
const char  *MY_L           (const char *text);

/////////////////////////////////////////////////////////////////////
//                       Inline definice
/////////////////////////////////////////////////////////////////////
inline void MY_FLoad (const char *fl, void *data)
{
    MY_FLoadBlock (fl, 0, -1, data);
}

inline size_t MY_FRead (void *buffer, size_t size, size_t count, myFile_s *f)
{
    return fread (buffer, size, count, f->file);
}

inline size_t MY_FWrite (const void *buffer, size_t size, size_t count, myFile_s *f)
{
    return fwrite (buffer, size, count, f->file);
}

#endif
