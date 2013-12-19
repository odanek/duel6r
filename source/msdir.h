/*
 * msdir.h - dirent port to Microsoft Windows and DOS
 *
 * Copyright (c) 1998 Toni Ronkko
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * ``Software''), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL TONI RONKKO BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * 28.06.1998, Toni Ronkko <tronkko@hytti.uku.fi>
 * Kehakuja 13, 73100 LAPINLAHTI, Finland.
 *
 * $Id: msdir.h,v 1.2 2001/06/13 19:13:30 pem Exp $
 *
 * $Log: msdir.h,v $
 * Revision 1.2  2001/06/13 19:13:30  pem
 * Minor changes to support MS VC++ compiler. Especialy explicit specification
 * of namespace std:: before some symbols.
 *
 * Revision 1.1  2001/03/29 15:53:16  pem
 * Added support for scannig directory
 *
 * Revision 1.2  1998/07/19 18:29:14  tr
 * Added error reporting capabilities and some asserts.
 *
 * Revision 1.1  1998/07/04 16:27:51  tr
 * Initial revision
 *
 *
 *
 *
 * Note:
 * - This port might not work correctly on MS-DOS if you create or remove
 *   files while scanning directories. I have not found a single situation
 *   that would not have worked, but I still suspect that it will break
 *   sometime (as DOS usually).
 * - Important note for MSC users: at least version 1.0 of MSC scans
 *   automatic dependencies incorrectly when project contains this header.
 *   It looks like that MSC could not handle include directives inside
 *   #if..#endif block those are never entered. Because this header ought to
 *   compile in many different operating systems, there had to be several
 *   conditional blocks that are compiled only in operating systems for what
 *   they were designed. Because of that, the confusion was un-avoidable.
 *   MSVC 1.0 cannot handle inclusion of sys/dir.h in a part that is compiled
 *   only in Apollo operating system, and thus refuses to co-operate.
 *   To fix this, you need to insert DIR.H into SYSINCL.DAT located in
 *   MSVC\BIN directory and restart visual C++. Consult manuals for more
 *   informaton about the problem.
 */
#ifndef MSDIR_H
#define MSDIR_H




/* Figure out whether to use port of dirent or real one */
#if !defined( USE_DIRENT_PORT)

  /* Use dirent.h if user says so */
# if defined( HAS_DIRENT_H)
#   include <dirent.h>

  /* MSC does not have dirent.h */
# elif defined(_MSC_VER)
#   define USE_DIRENT_PORT

  /* Watcom C/C++ has direct.h that is almost equivalent of dirent.h
   * with only rewinddir() missing. Let's use port in Windows and DOS. */
# elif defined(__WATCOMC__)  &&  (defined(__MSDOS__) || defined(__NT__))
#   define USE_DIRENT_PORT
# elif defined(__WATCOMC__)
#   include <direct.h>


  /* Borland C/C++ has dirent.h */
# elif defined( __BORLANDC__)
#   include <dirent.h>

  /* gcc has dirent.h */
# elif defined(__GNUC__)
#   include <dirent.h>

  /* Apollo has equivalent dir.h */
# elif defined(APOLLO)  ||  defined(__apollo)
#   include <sys/types.h>
    /* This is the place mentioned in prolog that confuses MSC */
#   include <sys/dir.h>
#   ifndef dirent
#     define dirent direct
#   endif

  /* Turbo C does not have dirent.h */
# elif defined( __TURBOC__)
#   define USE_DIRENT_PORT

  /* There is dirent.h in HP-UX but it requires that _HPUX_SOURCE is defined */
# elif defined(HP_UX)
#   if !defined(_HPUX_SOURCE)
#     define _HPUX_SOURCE
#   endif
#   include <dirent.h>

  /* Unknown compiler or dirent.h does not exist, try port */
# else
#   define USE_DIRENT_PORT

# endif
#endif






/****** Port of dirent.h ******/
#ifdef USE_DIRENT_PORT

/* Figure out type of underlaying interface used to implement port*/
# if defined(__BORLANDC__)  &&  defined(__WIN32__)
#   define MSDIR_WIN32_INTERFACE
# elif defined(__BORLANDC__)
#   define MSDIR_MSDOS_INTERFACE
# endif

# if defined(_MSC_VER)  &&  defined(_WIN32)
#   define MSDIR_WIN32_INTERFACE
# elif defined(_MSC_VER)
#   define MSDIR_MSDOS_INTERFACE
# endif

# if defined(__TURBOC__)  &&  defined(__MSDOS__)
#   define MSDIR_MSDOS_INTERFACE
# endif

# if defined(__WATCOMC__)  &&  defined(__NT__)
#   define MSDIR_WIN32_INTERFACE
# else
#   define MSDIR_MSDOS_INTERFACE
# endif

# if !defined(MSDIR_WIN32_INTERFACE)  &&  !defined(MSDIR_MSDOS_INTERFACE)
#   error "Unable to determinate interface for implementing dirent."
# endif

/* Use Win32 interface if both Win32 and DOS would do */
# if defined(MSDIR_WIN32_INTERFACE)  &&  defined(MSDIR_MSDOS_INTERFACE)
#   undef MSDIR_MSDOS_INTERFACE
# endif





# if defined(MSDIR_WIN32_INTERFACE)
#   include <windows.h>
#   if !defined(MSDIR_MAXPATH)
#     define MSDIR_MAXPATH (MAX_PATH)
#   endif

# elif defined(MSDIR_MSDOS_INTERFACE)
#   include <dos.h>

    /* Borland defines file length macros in dir.h */
#   if defined(__BORLANDC__)
#     include <dir.h>
#     ifndef MSDIR_MAXPATH
#       define MSDIR_MAXPATH ((MAXFILE)+(MAXEXT))
#     endif
#     ifndef _find_t
#       define _find_t find_t
#     endif

    /* Turbo C defines ffblk structure in dir.h */
#   elif defined( __TURBOC__)
#     include <dir.h>
#     ifndef MSDIR_MAXPATH
#       define MSDIR_MAXPATH ((MAXFILE)+(MAXEXT))
#     endif
#     define MSDIR_USE_FFBLK

    /* MSC */
#   elif defined(_MSC_VER)
#     ifndef MSDIR_MAXPATH
#       define MSDIR_MAXPATH (13)
#     endif

    /* Watcom */
#   elif defined(__WATCOMC__)
#     ifndef MSDIR_MAXPATH
#       if defined(__OS2__) || defined(__NT__)
#         define MSDIR_MAXPATH (256)
#       else
#         define MSDIR_MAXPATH (13)
#       endif
#     endif

#   endif
# endif


/* Use ffblk instead of _find_t if requested so */
# if defined(MSDIR_USE_FFBLK)
#   define _find_t ffblk
#   define _A_ARCH   (FA_ARCH)
#   define _A_HIDDEN (FA_HIDDEN)
#   define _A_NORMAL (0)
#   define _A_RDONLY (FA_RDONLY)
#   define _A_SUBDIR (FA_DIREC)
#   define _A_SYSTEM (FA_SYSTEM)
#   define _A_VOLID  (FA_LABEL)
#   define _dos_findnext(dest) findnext(dest)
#   define _dos_findfirst(name,flags,dest) findfirst(name,dest,flags)
# endif



/* Get file name from find structure */
# if defined(MSDIR_WIN32_INTERFACE)
#   define MSDIR_DATA_NAME(x) (x)->cFileName

# elif defined(MSDIR_USE_FFBLK)
#   define MSDIR_DATA_NAME(x) (x)->ff_name

# else
#   define MSDIR_DATA_NAME(x) (x)->name
# endif


/* dirent replacement */
typedef struct dirent
{
  char d_name[MSDIR_MAXPATH];

  /*** Operating system specific part ***/
# if defined(MSDIR_WIN32_INTERFACE)
  WIN32_FIND_DATA data;
# elif defined(MSDIR_MSDOS_INTERFACE)
  struct _find_t data;
# endif
} dirent;


/* DIR replacement structure */
typedef struct DIR
{
  char          *dirname;                      /* directory that is scanned */
  dirent        current;                       /* current file entry */
  int           dirent_filled;                 /* is current un-processed? */

  /*** Operating system specific part ***/
# if defined(MSDIR_WIN32_INTERFACE)
  HANDLE        search_handle;
# elif defined(MSDIR_MSDOS_INTERFACE)
# endif
} DIR;






/******* dirent replacement functions *******/
#ifdef __cplusplus
extern "C" {
#endif

/* see msdir.c */
DIR *opendir( const char *dirname);

struct dirent *readdir( DIR *dirp);

int closedir( DIR *dirp);

void rewinddir( DIR *dirp);

#ifdef __cplusplus
}
#endif


#endif /*USE_DIRENT_PORT*/
#endif /*MSDIR_H*/
