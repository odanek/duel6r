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
Projekt: Knihovna pro praci s kamerou
Popis: Hlavickovy soubor
*/

#ifndef __MYCAM_H
#define __MYCAM_H

#include "mymath.h"

/*
==================================================
trida myfrust_c
==================================================
*/
class myfrust_c
{
    private:
        vec3_c<mval_t>      m_bv[5], m_pos;
        mat3_c<mval_t>      m_matrix;
        plane3_c<mval_t>    m_plane[5];

    public:
                myfrust_c       (mval_t fov, mval_t aspect);
        void    updatepos       (const vec3_c<mval_t> &p);
        void    updatematrix    (const mat3_c<mval_t> &m);
        void    reset           (mval_t fov, mval_t aspect);
        bool    in              (const vec3_c<mval_t> &p) const;
        bool    in              (const bbox3_c<mval_t> &b) const;

    private:
        void    rebuild         ();
};

/*
==================================================
trida mycam_c
==================================================
*/
class mycam_c
{
    private:
        vec3_c<mval_t>  m_front, m_side, m_up, m_pos;
        mval_t          m_pitch, m_roll, m_heading;

    public:
        myfrust_c       *m_frustum;

        enum
        {
            CM_WALK,
            CM_FLY
        };

                mycam_c         ();
                ~mycam_c        ();
        void    setpos          (const vec3_c<mval_t> &p);
        void    getpos          (vec3_c<mval_t> &p) const;
        void    getdir          (vec3_c<mval_t> &front, vec3_c<mval_t> &side, vec3_c<mval_t> &up) const;
        void    move            (mval_t spd);
        void    strafe          (mval_t spd);
        void    rotate          (mval_t h, mval_t p, mval_t r);
        void    look            () const;
        void    facecam         () const;
        void    resize          (bool fr, mval_t fov, float a);

    private:
        void    makerotation    ();
};


#endif
