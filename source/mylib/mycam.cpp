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
Popis: Prace s kamerou
*/

#include <SDL2/SDL_opengl.h>
#include "mycam.h"

/*
==================================================

trida myfrust_c

==================================================
*/

/*
==================================================
konstruktor
==================================================
*/
myfrust_c::myfrust_c (mval_t fov, mval_t aspect)
{
    m_pos.set (0.0f, 0.0f, 0.0f);
    m_matrix.identity ();
    reset (fov, aspect);
}

/*
==================================================
nastavi frustum podle nove rotacni matice
==================================================
*/
void myfrust_c::updatematrix (const mat3_c<mval_t> &m)
{
    m_matrix = m;
    rebuild ();
}

/*
==================================================
nastavi frustum na danou pozici v prostoru
==================================================
*/
void myfrust_c::updatepos (const vec3_c<mval_t> &p)
{
    m_pos = p;
    rebuild ();
}

/*
==================================================
zmena "field of view" nebo "aspect ratio"
==================================================
*/
void myfrust_c::reset (mval_t fov, mval_t aspect)
{
    mval_t  a, cA, sA;

    a = (mval_t) MM_D2R (fov / 2.0);
    sA = (mval_t) sin (a);
    cA = (mval_t) cos (a);

    m_bv[0].set (0.0, 0.0, 1.0);
    m_bv[1].set (cA, 0.0, sA * aspect);
    m_bv[2].set (-cA, 0.0, sA * aspect);
    m_bv[3].set (0.0, cA, sA);
    m_bv[4].set (0.0, -cA, sA);

    m_bv[1].normalize ();
    m_bv[2].normalize ();

    rebuild ();
}

/*
==================================================
vypocte frustum
==================================================
*/
void myfrust_c::rebuild ()
{
    for (int i = 0; i < 5; i++)
    {
        m_plane[i].m_normal = m_matrix * m_bv[i];
        m_plane[i].calcdist (m_pos);
    }
}

/*
==================================================
zjisti zda se bod nachazi uvnitr
==================================================
*/
bool myfrust_c::in (const vec3_c<mval_t> &p) const
{
    for (int i = 1; i < 5; i++)
        if (!m_plane[i].isabove (p))
            return false;

    return true;
}

/*
==================================================
zjisti zda se aabb nachazi uvnitr
==================================================
*/
bool myfrust_c::in (const bbox3_c<mval_t> &b) const
{
    mval_t                  n;
    const vec3_c<mval_t>    *v;
    int                     i, j;

    for (j = 0; j < 5; j++)
    {
        v = &m_plane[j].m_normal;
        n = m_plane[j].m_dist;

        for (i = 0; i < 8; i++)
            if ((*v) * b.corner[i] >= n)
                break;

        if (i >= 8)
            return false;
    }

    return true;
}

/*
==================================================

trida mycam_c

==================================================
*/

/*
==================================================
konstruktor
==================================================
*/
mycam_c::mycam_c ()
{
    m_pos.set (0.0f, 0.0f, 0.0f);
    m_side.set (1.0f, 0.0f, 0.0f);
    m_up.set (0.0f, 1.0f, 0.0f);
    m_front.set (0.0f, 0.0f, 1.0f);

    m_heading = (mval_t) 0.0;
    m_pitch = (mval_t) 0.0;
    m_roll = (mval_t) 0.0;

    m_frustum = NULL;
}

/*
==================================================
destruktor
==================================================
*/
mycam_c::~mycam_c ()
{
    if (m_frustum != NULL)
        delete m_frustum;
}

/*
==================================================
zmena "field of view", pripadne "aspect ratio"
==================================================
*/
void mycam_c::resize (bool fr, mval_t fov, float a)
{
    if (m_frustum != NULL)
        delete m_frustum;

    if (fr)
        m_frustum = new myfrust_c (fov, a);
}

/*
==================================================
nastavi kameru na pozici v prostoru
==================================================
*/
void mycam_c::setpos (const vec3_c<mval_t> &p)
{
    m_pos = p;
    if (m_frustum != NULL)
        m_frustum->updatepos (m_pos);
}

/*
==================================================
vrati pozici kamery v prostoru
==================================================
*/
void mycam_c::getpos (vec3_c<mval_t> &p) const
{
    p = m_pos;
}

/*
==================================================
pohne kamerou ve smeru jejiho pohledu
==================================================
*/
void mycam_c::move (mval_t spd)
{
    m_pos += spd * m_front;

    if (m_frustum != NULL)
        m_frustum->updatepos (m_pos);
}

/*
==================================================
pohne kamerou do strany
==================================================
*/
void mycam_c::strafe (mval_t spd)
{
    m_pos += spd * m_side;

    if (m_frustum != NULL)
        m_frustum->updatepos (m_pos);
}

/*
==================================================
opengl transformace podle pohledu kamery
==================================================
*/
void mycam_c::look () const
{
    //gluLookAt (m_pos.x, m_pos.y, m_pos.z,
    //           m_pos.x + m_front.x, m_pos.y + m_front.y, m_pos.z + m_front.z,
    //           m_up.x, m_up.y, m_up.z);
    vec3_c<mval_t> s = m_front % m_up;
    vec3_c<mval_t> u = s % m_front;
    
    mat4_c<mval_t> p(0.0f);
    p(0,0) = s.x; p(1,0) = s.y; p(2,0) = s.z; p(3,0) = 0.0f;
    p(0,1) = u.x; p(1,1) = u.y; p(2,1) = u.z; p(3,1) = 0.0f;
    p(0,2) = -m_front.x; p(1,2) = -m_front.y; p(2,2) = -m_front.z; p(3,2) = 0.0f;
    p(0,3) = 0.0f; p(1,3) = 0.0f; p(2,3) = 0.0f; p(3,3) = 1.0f;

    glMultMatrixf(&p(0,0));
    glTranslatef(-m_pos.x, -m_pos.y, -m_pos.z);
}

/*
==================================================
nastavi vektory kamery podle rotace
==================================================
*/
void mycam_c::makerotation ()
{
    mat3_c<mval_t>  m;
    mval_t          a, b, c, d, e, f;

    a = (mval_t) sin (m_pitch);
    b = (mval_t) cos (m_pitch);
    c = (mval_t) sin (m_heading);
    d = (mval_t) cos (m_heading);
    e = (mval_t) sin (m_roll);
    f = (mval_t) cos (m_roll);

    m.d[0] = d * f + c * a * e;
    m.d[1] = -d * e + c * a * f;
    m.d[2] = c * b;
    m.d[3] = e * b;
    m.d[4] = f * b;
    m.d[5] = -a;
    m.d[6] = -f * c + d * a * e;
    m.d[7] = c * e + d * a * f;
    m.d[8] = b * d;

    m_side.set (m.d[0], m.d[3], m.d[6]);
    m_up.set (m.d[1], m.d[4], m.d[7]);
    m_front.set (m.d[2], m.d[5], m.d[8]);

    if (m_frustum != NULL)
        m_frustum->updatematrix (m);
}

/*
==================================================
otoci kamerou o dane uhly
==================================================
*/
void mycam_c::rotate (mval_t h, mval_t p, mval_t r)
{
    m_heading += (mval_t) MM_D2R (h);
    m_pitch += (mval_t) MM_D2R (p);
    m_roll += (mval_t) MM_D2R (r);

    makerotation ();
}

/*
==================================================
je treba volat pred kreslenim "billboard" polygonu
==================================================
*/
void mycam_c::facecam () const
{
    glRotatef (MM_R2D (m_heading), 0, 1.0f, 0);
    glRotatef (MM_R2D (m_pitch), 1.0f, 0, 0);
    glRotatef (MM_R2D (m_roll), 0, 0, 1.0f);
    glRotatef (-180, 0, 1.0f, 0);
}

/*
==================================================
vraci smerove vektory kamery
==================================================
*/
void mycam_c::getdir (vec3_c<mval_t> &front, vec3_c<mval_t> &side, vec3_c<mval_t> &up) const
{
    front = m_front;
    side = m_side;
    up = m_up;
}
