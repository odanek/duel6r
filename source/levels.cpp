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

#include "msdir.h"
#include "project.h"

void d6LEVELS_c::Init(const char *dir)
{
    DIR                 *handle;
    struct dirent       *ff;    
    size_t              dnlen;

    // Allocate room for level file names
    m_lev = (char **)MY_Alloc(D6_LEVELS_MAX * sizeof(char *));
    m_name = (char **)MY_Alloc(D6_LEVELS_MAX * sizeof(char *));
    m_init = true;

    dnlen = strlen(dir);
    handle = opendir(dir);
    ff = (handle == NULL) ? NULL : readdir (handle);

    while (ff != NULL && m_count < D6_LEVELS_MAX)
    {
        if (ff->d_name[0] != '.')
        {
            size_t fnlen = strlen(ff->d_name);

            if (fnlen > 4)
            {
                if (!strcmp (".lev", ff->d_name + fnlen - 4))
                {
                    m_lev[m_count] = (char *)MY_Alloc((dnlen + fnlen + 1) * sizeof(char));
                    m_name[m_count] = (char *)MY_Alloc((fnlen - 3) * sizeof(char));
                    sprintf(m_lev[m_count], "%s%s", dir, ff->d_name);
                    strncpy(m_name[m_count], ff->d_name, fnlen - 4);
                    m_name[m_count][fnlen - 4] = 0;
                    m_count++;
                }
            }
        }

        ff = readdir (handle);
    }

    closedir (handle);
}

void d6LEVELS_c::DeInit()
{
    if (m_init)
    {
        for (size_t i = 0; i < m_count; i++)
        {
            MY_Free(m_lev[i]);
            MY_Free(m_name[i]);
        }

        m_count = 0;
        MY_Free(m_lev);
        MY_Free(m_name);

        m_init = false;
    }
}