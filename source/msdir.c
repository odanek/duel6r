/*
* msdir.c - dirent port to Microsoft Windows and DOS
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
* $Id: msdir.c,v 1.2 2006/07/26 14:50:33 pem Exp $
*
* $Log: msdir.c,v $
* Revision 1.2  2006/07/26 14:50:33  pem
* *** empty log message ***
*
* Revision 1.1  2001/03/29 15:53:16  pem
* Added support for scannig directory
*
* Revision 1.2  1998/07/19 18:29:14  tr
* Added error reporting capabilities and some asserts.
*
* Revision 1.1  1998/07/04 16:27:39  tr
* Initial revision
*
*
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "msdir.h"
#ifdef USE_DIRENT_PORT

static int init_DIR( DIR *p);

/*
* Allocate workspace and open directory stream. If either one fails,
* NULL will be returned.
*/
DIR *opendir( const char *dirname)
{
    DIR *dirp;
    assert( dirname != NULL);

    dirp = (DIR*)malloc( sizeof( struct DIR));
    if( dirp != NULL)
    {
        char *p;

        /* Allocate room for directory name */
        dirp->dirname = (char*) malloc( strlen( dirname) + 1 + strlen( "\\*.*"));
        if( dirp->dirname == NULL)
        {
            /* Failed to duplicate directory name */
            free( dirp);
            return NULL;
        }
        /*
        * Copy directory name while appending path separator and "*.*". Path
        * separator is not appended if path ends in drive or directory separator.
        */
        strcpy( dirp->dirname, dirname);
        p = strchr( dirp->dirname, '\0');
        if( dirp->dirname < p  &&
            *(p - 1) != '\\'  &&  *(p - 1) != '/'  &&  *(p - 1) != ':')
        {
            strcpy( p++, "\\");
        }
        strcpy( p, "*.*");

        /* Open stream */
        if( init_DIR( dirp) == 0)
        {
            /* Initialization failed */
            free( dirp->dirname);
            free( dirp);
            return NULL;
        }
    }
    return dirp;
}

/*
* Return previously retrieved file or get next one. (real dirent has
* separate open and read functions, but Win32 and DOS interface will open
* stream automatically when we retrieve first file. So, we fetch the first
* file when opening stream and return it later when user tries to read first
* entry.)
*/
struct dirent *readdir( DIR *dirp)
{
    assert( dirp != NULL);
#ifndef OPTIMIZE_SIZE
    if( dirp == NULL)
    {
        errno = EBADF;
        return NULL;
    }
#endif
#if defined(MSDIR_WIN32_INTERFACE)
    if( dirp->search_handle == INVALID_HANDLE_VALUE)
    {
        /* Directory stream was opened/rewound incorrectly or it ended earlier */
        errno = EBADF;
        return NULL;
    }
#endif


    if( dirp->dirent_filled != 0)
    {
        /* Entry was filled earlier and there is no need to retrieve new */
        dirp->dirent_filled = 0;
    }
    else
    {
        /* Fill in entry and return that */
#if defined(MSDIR_WIN32_INTERFACE)
        if( FindNextFile( dirp->search_handle, &dirp->current.data) == FALSE)
        {
            /* Last file has been processed or an error occured */
            FindClose( dirp->search_handle);
            dirp->search_handle = INVALID_HANDLE_VALUE;
            errno = ENOENT;
            return NULL;
        }

# elif defined(MSDIR_MSDOS_INTERFACE)
        if( _dos_findnext( &dirp->current.data) != 0)
        {
            /*
            * _dos_findnext and findnext will set errno to ENOENT when no
            * more entries could be retrieved.
            */
            return NULL;
        }
# endif

        /* make sure that d_name is long enough. Turbo C 2.0 will complain about
        * unreachable code at line below */
        assert( sizeof(dirp->current.d_name) >=
            sizeof( MSDIR_DATA_NAME(&dirp->current.data)) );

        strcpy( dirp->current.d_name, MSDIR_DATA_NAME(&dirp->current.data));
        assert( dirp->dirent_filled == 0);
    }
    return &dirp->current;
}





/*
* Close directory stream
*/
int closedir( DIR *dirp)
{
    int retcode = 0;

    /* Make sure that dirp points to legal structure */
    assert( dirp != NULL);
#ifndef OPTIMIZE_SIZE
    if( dirp == NULL)
    {
        errno = EBADF;
        return -1;
    }
#endif


    /* Free directory name and search handles */
    if( dirp->dirname != NULL) free( dirp->dirname);

#if defined(MSDIR_WIN32_INTERFACE)
    if( dirp->search_handle != INVALID_HANDLE_VALUE)
    {
        if( FindClose( dirp->search_handle) == FALSE)
        {
            /* Unknown error */
            retcode = -1;
            errno = EBADF;
        }
    }
#endif


    /* Clear dirp structure to make sure that it cannot be used anymore*/
#ifndef OPTIMIZE_SIZE
    memset( dirp, 0, sizeof( *dirp));
# if defined(MSDIR_WIN32_INTERFACE)
    dirp->search_handle = INVALID_HANDLE_VALUE;
# endif
#endif

    free( dirp);
    return retcode;
}






/*
* Rewind directory stream to beginning
*/
void rewinddir( DIR *dirp)
{
    /* Make sure that dirp is legal */
    assert( dirp != NULL);
#ifndef OPTIMIZE_SIZE
    if( dirp == NULL)
    {
        errno = EBADF;
        return;
    }
#endif
    assert( dirp->dirname != NULL);


    /* Close previous stream */
#if defined(MSDIR_WIN32_INTERFACE)
    if( dirp->search_handle != INVALID_HANDLE_VALUE)
    {
        if( FindClose( dirp->search_handle) == FALSE)
        {
            /* Unknown error */
            errno = EBADF;
        }
    }
#endif


    /* Re-open previous stream */
    if( init_DIR( dirp) == 0)
    {
        /* Initialization failed but we cannot deal with error. User will notice
        * error later when she tries to retrieve first directory enty. */
    }
}








/****** Utility functions ******/


/*
* Initialize DIR structure with new directory. The function
* returns 0 on failure and nonzero on success.
*
* Note:
* - Be sure to close previous stream before opening new one
*/
static int init_DIR( DIR *dirp)
{
    assert( dirp != NULL);
    assert( dirp->dirname != NULL);
    dirp->dirent_filled = 0;

# if defined(MSDIR_WIN32_INTERFACE)
    /* Open stream and retrieve first file */
    dirp->search_handle = FindFirstFile( dirp->dirname, &dirp->current.data);
    if( dirp->search_handle == INVALID_HANDLE_VALUE)
    {
        /*
        * Something went wrong but we don't know what. GetLastError() could
        * give us more information about the error, but then we should map
        * the error code into errno.
        */
        errno = ENOENT;
        return 0;
    }

# elif defined(MSDIR_MSDOS_INTERFACE)
    if( _dos_findfirst( dirp->dirname,
        _A_SUBDIR | _A_RDONLY | _A_ARCH | _A_SYSTEM | _A_HIDDEN | _A_VOLID,
        &dirp->current.data) != 0)
    {
        /*
        * _dos_findfirst and findfirst will set errno to ENOENT when no
        * more entries could be retrieved.
        */
        return 0;
    }
# endif

    /* make sure that d_name is long enough. Turbo C 2.0 will complain about
    * unreachable code at line below */
    assert( sizeof(dirp->current.d_name) >=
        sizeof( MSDIR_DATA_NAME(&dirp->current.data)) );


    /* Initialize DIR and it's first entry */
    strcpy( dirp->current.d_name, MSDIR_DATA_NAME(&dirp->current.data));
    dirp->dirent_filled = 1;
    return 1;
}


#endif /*USE_DIRENT_PORT*/
