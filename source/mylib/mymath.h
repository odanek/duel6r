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
Popis: Matematicke operace, vektory, matice, ...
*/

#ifndef __MYMATH_H
#define __MYMATH_H

#include <math.h>
#include <string.h>

// Presnost knihovny
typedef float   mval_t;

#define MM_D2R(x)       ((x) * (mval_t) 0.017453292)
#define MM_R2D(x)       ((x) * (mval_t) 57.29577951)
#define MM_PI           (mval_t) 3.14159265358979323846

// Forward deklarace
template<class T, int R, int S> class matrix_c;

/*
==================================

    VEKTORY

==================================
*/

/*
==================================================
vec_c
==================================================
*/
template<class T, int N> class vec_c
{
public:
    T   d[N];

    // Konstruktory
    vec_c       () {};
    vec_c       (T val) { set (val); }
    vec_c       (const vec_c<T,N> &v) { *this = v; }

    // Funkce
    mval_t          len         () const;
    vec_c&          normalize   ();
    matrix_c<T,1,N> mat_h       () const { matrix_c<T,1,N> m; memcpy (m.d, d, sizeof (d)); return m; }
    matrix_c<T,N,1> mat_v       () const { matrix_c<T,N,1> m; memcpy (m.d, d, sizeof (d)); return m; }
    void            set         (T vx) { for (int i = 0; i < N; i++) d[i] = vx; }
    void            set         (const T *v) { memcpy (d, v, sizeof (d)); }

    // Operatory
    vec_c       operator+   (const vec_c &v) const;
    vec_c&      operator+=  (const vec_c &v) { for (int i = 0; i < N; i++) d[i] += v.d[i]; return *this; }
    vec_c       operator-   () const;
    vec_c       operator-   (const vec_c &v) const;
    vec_c&      operator-=  (const vec_c &v) { for (int i = 0; i < N; i++) d[i] -= v.d[i]; return *this; }
    vec_c       operator*   (T n) const;
    vec_c&      operator*=  (T n) { for (int i = 0; i < N; i++) d[i] *= n; return *this; }
    vec_c       operator/   (T n) const;
    vec_c&      operator/=  (T n) { for (int i = 0; i < N; i++) d[i] /= n; return *this; }
    T&          operator[]  (int n) { return d[n]; }
    const T&    operator[]  (int n) const { return d[n]; }
    bool        operator==  (const vec_c &v) const;
    bool        operator!=  (const vec_c &v) const;
    T           operator*   (const vec_c &v) const;
};

template<class T, int N> mval_t vec_c<T,N>::len () const
{
    T   sum = 0;

    for (int i = 0; i < N; i++)
        sum += d[i];
    return sqrt ((mval_t) sum);
}

template<class T, int N> inline vec_c<T,N>& vec_c<T,N>::normalize ()
{
    mval_t  l = len ();
    for (int i = 0; i < N; i++)
        d[i] /= l;
    return *this;
}

template<class T, int N> inline vec_c<T,N> vec_c<T,N>::operator+ (const vec_c<T,N> &v) const
{
    vec_c<T,N>  a;
    for (int i = 0; i < N; i++)
        a.d[i] = d[i] + v.d[i];
    return a;
}

template<class T, int N> inline vec_c<T,N> vec_c<T,N>::operator- () const
{
    vec_c<T,N>  a;
    for (int i = 0; i < N; i++)
        a.d[i] = -d[i];
    return a;
}

template<class T, int N> inline vec_c<T,N> vec_c<T,N>::operator- (const vec_c<T,N> &v) const
{
    vec_c<T,N>  a;
    for (int i = 0; i < N; i++)
        a.d[i] = d[i] - v.d[i];
    return a;
}

template<class T, int N> inline vec_c<T,N> vec_c<T,N>::operator* (T n) const
{
    vec_c<T,N>  a;
    for (int i = 0; i < N; i++)
        a.d[i] = d[i] * n;
    return a;
}

template<class T, int N> inline vec_c<T,N> vec_c<T,N>::operator/ (T n) const
{
    vec_c<T,N>  a;
    for (int i = 0; i < N; i++)
        a.d[i] = d[i] / n;
    return a;
}

template<class T, int N> inline bool vec_c<T,N>::operator== (const vec_c<T,N> &v) const
{
    for (int i = 0; i < N; i++)
        if (d[i] != v.d[i])
            return false;
    return true;
}

template<class T, int N> inline bool vec_c<T,N>::operator!= (const vec_c<T,N> &v) const
{
    for (int i = 0; i < N; i++)
        if (d[i] != v.d[i])
            return true;
    return false;
}

template<class T, int N> inline T vec_c<T,N>::operator* (const vec_c<T,N> &v) const
{
    T   sum = 0;

    for (int i = 0; i < N; i++)
        sum += d[i] * v.d[i];
    return sum;
}

template<class T, int N> inline vec_c<T,N> operator* (T n, const vec_c<T,N> &v)
{
    vec_c<T,N>  a;
    for (int i = 0; i < N; i++)
        a.d[i] = v.d[i] * n;
    return a;
}

/*
==================================
vec2_c
==================================
*/
template<class T> class vec2_c
{
public:
    T   x, y;

    // Konstruktory
    vec2_c      () {};
    vec2_c      (T val) : x(val), y(val) {}
    vec2_c      (const vec2_c &v) : x(v.x), y(v.y) {}
    vec2_c      (const vec_c<T,2> &v) : x(v.d[0]), y(v.d[1]) {}
    vec2_c      (T vx, T vy) : x(vx), y(vy) {}
    vec2_c      (T *v) { set (v); }

    // Funkce
    mval_t          len         () const { return sqrt ((mval_t) (x * x + y * y)); }
    vec2_c&         normalize   ();
    vec_c<T,2>      vec         () const { vec_c<T,2> v; v.d[0] = x; v.d[1] = y; return v; }
    matrix_c<T,1,2> mat_h       () const { matrix_c<T,1,2> m; m.d[0] = x; m.d[1] = y; return m; }
    matrix_c<T,2,1> mat_v       () const { matrix_c<T,2,1> m; m.d[0] = x; m.d[1] = y; return m; }
    void            set         (T vx, T vy) { x = vx; y = vy; }
    void            set         (const T *v) { x = v[0]; y = v[1]; }

    // Operatory
    vec2_c&     operator=   (const vec_c<T,2> &v) { x = v.d[0]; y = v.d[1]; return *this; }
    vec2_c      operator+   (const vec2_c &v) const { return vec2_c<T> (x + v.x, y + v.y); }
    vec2_c&     operator+=  (const vec2_c &v) { x += v.x; y += v.y; return *this; }
    vec2_c      operator-   () const { return vec2_c<T> (-x, -y); }
    vec2_c      operator-   (const vec2_c &v) const { return vec2_c<T> (x - v.x, y - v.y);  }
    vec2_c&     operator-=  (const vec2_c &v) { x -= v.x; y -= v.y; return *this; }
    vec2_c      operator*   (T n) const { return vec2_c<T> (x * n, y * n); }
    vec2_c&     operator*=  (T n) { x *= n; y *= n; return *this; }
    vec2_c      operator/   (T n) const { return vec2_c<T> (x / n, y / n); }
    vec2_c&     operator/=  (T n) { x /= n; y /= n; return *this; }
    bool        operator==  (const vec2_c &v) const { return x == v.x && y == v.y; }
    bool        operator!=  (const vec2_c &v) const { return x != v.x || y != v.y; }
    T           operator*   (const vec2_c &v) const { return x * v.x + y * v.y; }
};

template<class T> inline vec2_c<T>& vec2_c<T>::normalize ()
{
    mval_t l = len ();
    x = (T) (x / l);
    y = (T) (y / l);
    return *this;
}

template<class T> inline vec2_c<T> operator* (T n, const vec2_c<T> &v)
{
    return vec2_c<T> (v.x * n, v.y * n);
}

/*
==================================
vec3_c
==================================
*/
template<class T> class vec3_c
{
public:
    T   x, y, z;

    // Konstruktory
    vec3_c      () {};
    vec3_c      (T val) : x(val), y(val), z(val) {}
    vec3_c      (const vec3_c &v) : x(v.x), y(v.y), z(v.z) {}
    vec3_c      (const vec_c<T,3> &v) : x(v.d[0]), y(v.d[1]), z(v.d[2]) {}
    vec3_c      (T vx, T vy, T vz) : x(vx), y(vy), z(vz) {}
    vec3_c      (T *v) { set (v); }

    // Funkce
    mval_t          len         () const { return sqrt ((mval_t) (x * x + y * y + z * z)); }
    vec3_c&         normalize   ();
    vec_c<T,3>      vec         () const { vec_c<T,3> v; v.d[0] = x; v.d[1] = y; v.d[2] = z; return v; }
    matrix_c<T,1,3> mat_h       () const { matrix_c<T,1,3> m; m.d[0] = x; m.d[1] = y; m.d[2] = z; return m; }
    matrix_c<T,3,1> mat_v       () const { matrix_c<T,3,1> m; m.d[0] = x; m.d[1] = y; m.d[2] = z; return m; }
    void            set         (T vx, T vy, T vz) { x = vx; y = vy; z = vz; }
    void            set         (const T *v) { x = v[0]; y = v[1]; z = v[2]; }

    // Operatory
    vec3_c&     operator=   (const vec_c<T,3> &v) { x = v.d[0]; y = v.d[1]; z = v.d[2]; return *this; }
    vec3_c      operator+   (const vec3_c &v) const { return vec3_c<T> (x + v.x, y + v.y, z + v.z); }
    vec3_c&     operator+=  (const vec3_c &v) { x += v.x; y += v.y; z += v.z; return *this; }
    vec3_c      operator-   () const { return vec3_c<T> (-x, -y, -z); }
    vec3_c      operator-   (const vec3_c &v) const { return vec3_c<T> (x - v.x, y - v.y, z - v.z); }
    vec3_c&     operator-=  (const vec3_c &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    vec3_c      operator*   (T n) const { return vec3_c<T> (x * n, y * n, z * n); }
    vec3_c&     operator*=  (T n) { x *= n; y *= n; z *= n; return *this; }
    vec3_c      operator/   (T n) const { return vec3_c<T> (x / n, y / n, z / n); }
    vec3_c&     operator/=  (T n) { x /= n; y /= n; z /= n; return *this; }
    bool        operator==  (const vec3_c &v) const { return x == v.x && y == v.y && z == v.z; }
    bool        operator!=  (const vec3_c &v) const { return x != v.x || y != v.y || z != v.z; }
    T           operator*   (const vec3_c &v) const { return x * v.x + y * v.y + z * v.z; }
    vec3_c      operator%   (const vec3_c &v) const;
};

template<class T> inline vec3_c<T>& vec3_c<T>::normalize ()
{
    mval_t l = len ();
    x = (T) (x / l);
    y = (T) (y / l);
    z = (T) (z / l);
    return *this;
}

template<class T> inline vec3_c<T> vec3_c<T>::operator% (const vec3_c<T> &v) const
{
    return vec3_c<T> (y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

template<class T> inline vec3_c<T> operator* (T n, const vec3_c<T> &v)
{
    return vec3_c<T> (v.x * n, v.y * n, v.z * n);
};

/*
==================================
vec4_c
==================================
*/
template<class T> class vec4_c
{
public:
    T   x, y, z, w;

    // Konstruktory
    vec4_c      () {};
    vec4_c      (T val) : x(val), y(val), z(val), w((T) 1) {}
    vec4_c      (const vec4_c &v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    vec4_c      (const vec_c<T,4> &v) : x(v.d[0]), y(v.d[1]), z(v.d[2]), w(v.d[3]) {}
    vec4_c      (T vx, T vy, T vz, T vw = (T) 1) : x(vx), y(vy), z(vz), w(vw) {}
    vec4_c      (T *v) { set (v); }

    // Funkce
    mval_t          len         () const { return sqrt ((mval_t) (x * x + y * y + z * z)); }
    vec4_c&         normalize   ();
    vec_c<T,4>      vec         () const { vec_c<T,4> v; v.d[0] = x; v.d[1] = y; v.d[2] = z; v.d[3] = w; return v; }
    matrix_c<T,1,4> mat_h       () const { matrix_c<T,1,4> m; m.d[0] = x; m.d[1] = y; m.d[2] = z; m.d[3] = w; return m; }
    matrix_c<T,4,1> mat_v       () const { matrix_c<T,4,1> m; m.d[0] = x; m.d[1] = y; m.d[2] = z; m.d[3] = w; return m; }
    void            set         (T vx, T vy, T vz, T vw = (T) 1) { x = vx; y = vy; z = vz; w = vw; }
    void            set         (const T *v) { x = v[0]; y = v[1]; z = v[2]; w = v[3]; }

    // Operatory
    vec4_c&     operator=   (const vec_c<T,4> &v) { x = v.d[0]; y = v.d[1]; z = v.d[2]; w = v.d[3]; return *this; }
    vec4_c      operator+   (const vec4_c &v) const { return vec4_c<T> (x + v.x, y + v.y, z + v.z, w + v.w); }
    vec4_c&     operator+=  (const vec4_c &v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    vec4_c      operator-   () const { return vec4_c<T> (-x, -y, -z, -w); }
    vec4_c      operator-   (const vec4_c &v) const { return vec4_c<T> (x - v.x, y - v.y, z - v.z, w - v.w); }
    vec4_c&     operator-=  (const vec4_c &v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    vec4_c      operator*   (T n) const { return vec4_c<T> (x * n, y * n, z * n, w * n); }
    vec4_c&     operator*=  (T n) { x *= n; y *= n; z *= n; w *= n; return *this; }
    vec4_c      operator/   (T n) const { return vec4_c<T> (x / n, y / n, z / n, w / n); }
    vec4_c&     operator/=  (T n) { x /= n; y /= n; z /= n; w /= n; return *this; }
    bool        operator==  (const vec4_c &v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    bool        operator!=  (const vec4_c &v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }
    T           operator*   (const vec4_c &v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
};

template<class T> inline vec4_c<T>& vec4_c<T>::normalize ()
{
    x /= w;
    y /= w;
    z /= w;
    w = (T) 1;
    return *this;
}

template<class T> inline vec4_c<T> operator* (T n, const vec4_c<T> &v)
{
    return vec4_c<T> (v.x * n, v.y * n, v.z * n, v.w * n);
};

/*
==================================

    MATICE

==================================
*/

/*
==================================================
matrix_c
==================================================
*/
template<class T, int R, int S> class matrix_c
{
public:
    T   d[R * S];

    // Kontstruktory
    matrix_c    () {};
    matrix_c    (T val) { set (val); }
    matrix_c    (const matrix_c &m) { *this = m; }

    // Funkce
    matrix_c&   identity        ();
    void        set             (T val) { for (int i = R * S - 1; i >= 0; i--) d[i] = val; }
    void        set             (const T *v) { memcpy (d, v, sizeof (d)); }
    vec_c<T,R>  col             (int n) const;
    vec_c<T,S>  row             (int n) const;

    // Operatory
    matrix_c<T,R,S>     operator+       (const matrix_c<T,R,S> &m) const;
    matrix_c<T,R,S>&    operator+=      (const matrix_c<T,R,S> &m);
    matrix_c<T,R,S>     operator-       () const;
    matrix_c<T,R,S>     operator-       (const matrix_c<T,R,S> &m) const;
    matrix_c<T,R,S>&    operator-=      (const matrix_c<T,R,S> &m);
    matrix_c<T,R,S>     operator*       (T n) const;
    matrix_c<T,R,S>&    operator*=      (T n);
    matrix_c<T,R,S>     operator/       (T n) const;
    matrix_c<T,R,S>&    operator/=      (T n);
    bool                operator==      (const matrix_c<T,R,S> &m) const;
    bool                operator!=      (const matrix_c<T,R,S> &m) const;

    T&              operator()      (int r, int s) { return d[r * S + s]; }
    const T&        operator()      (int r, int s) const { return d[r * S + s]; }
    matrix_c<T,S,R> operator~       () const;
};

template<class T, int R, int S>
    inline matrix_c<T,R,S>& matrix_c<T,R,S>::identity ()
{
    int     i, j, p = 0;

    for (i = 0; i < R; i++)
        for (j = 0; j < S; j++)
            d[p++] = (T) ((i == j) ? 1 : 0);

    return *this;
}

template<class T, int R, int S>
    inline vec_c<T,R> matrix_c<T,R,S>::col (int n) const
{
    vec_c<T,R>  v;
    int         i, p = n;

    for (i = 0; i < R; i++, p += S)
        v.d[i] = d[p];

    return v;
}

template<class T, int R, int S>
    inline vec_c<T,S> matrix_c<T,R,S>::row (int n) const
{
    vec_c<T,S>  v;
    int         i, p = n * S;

    for (i = 0; i < S; i++, p++)
        v.d[i] = d[p];

    return v;
}

template<class T, int R, int S>
    inline matrix_c<T,R,S> matrix_c<T,R,S>::operator+ (const matrix_c<T,R,S> &m) const
{
    matrix_c<T,R,S> a;
    int             i = R * S;

    while (i-- > 0)
        a.d[i] = d[i] + m.d[i];
    return a;
}

template<class T, int R, int S>
    inline matrix_c<T,R,S>& matrix_c<T,R,S>::operator+= (const matrix_c<T,R,S> &m)
{
    int     i = R * S;

    while (i-- > 0)
        d[i] += m.d[i];
    return *this;
}

template<class T, int R, int S>
    inline matrix_c<T,R,S> matrix_c<T,R,S>::operator- (const matrix_c<T,R,S> &m) const
{
    matrix_c<T,R,S> a;
    int             i = R * S;

    while (i-- > 0)
        a.d[i] = d[i] - m.d[i];
    return a;
}

template<class T, int R, int S>
    inline matrix_c<T,R,S> matrix_c<T,R,S>::operator- () const
{
    matrix_c<T,R,S> a;
    int             i = R * S;

    while (i-- > 0)
        a.d[i] = -d[i];
    return a;
}

template<class T, int R, int S>
    inline matrix_c<T,R,S>& matrix_c<T,R,S>::operator-= (const matrix_c<T,R,S> &m)
{
    int     i = R * S;

    while (i-- > 0)
        d[i] -= m.d[i];
    return *this;
}

template<class T, int R, int S>
    inline matrix_c<T,R,S> matrix_c<T,R,S>::operator* (T n) const
{
    matrix_c<T,R,S> a;
    int             i = R * S;

    while (i-- > 0)
        a.d[i] = d[i] * n;
    return a;
}

template<class T, int R, int S>
    inline matrix_c<T,R,S>& matrix_c<T,R,S>::operator*= (T n)
{
    int     i = R * S;

    while (i-- > 0)
        d[i] *= n;
    return *this;
}

template<class T, int R, int S>
    inline matrix_c<T,R,S> matrix_c<T,R,S>::operator/ (T n) const
{
    matrix_c<T,R,S> a;
    int             i = R * S;

    while (i-- > 0)
        a.d[i] = d[i] / n;
    return a;
}

template<class T, int R, int S>
    inline matrix_c<T,R,S>& matrix_c<T,R,S>::operator/= (T n)
{
    int     i = R * S;

    while (i-- > 0)
        d[i] /= n;
    return *this;
}

template<class T, int R, int S>
    inline bool matrix_c<T,R,S>::operator== (const matrix_c<T,R,S> &m) const
{
    int     i = R * S;
    while (i-- > 0)
        if (d[i] != m.d[i])
            return false;
    return true;
}

template<class T, int R, int S>
    inline bool matrix_c<T,R,S>::operator!= (const matrix_c<T,R,S> &m) const
{
    int     i = R * S;
    while (i-- > 0)
        if (d[i] != m.d[i])
            return true;
    return false;
}

template<class T, int R, int S>
    matrix_c<T,S,R> matrix_c<T,R,S>::operator~ () const
{
    matrix_c<T,S,R> n;
    int             i, j, p = 0, p2 = 0;

    for (i = 0; i < R; i++)
    {
        p2 = i;
        for (j = 0; j < S; j++)
        {
            n.d[p2] = d[p++];
            p2 += R;
        }
    }

    return n;
}

template<class T, int A, int B, int C>
    matrix_c<T,A,C> operator* (const matrix_c<T,A,B> &m1, const matrix_c<T,B,C> &m2)
{
    matrix_c<T,A,C> n;
    int             i, j, k;
    T               v, *p = n.d;
    const T         *p1, *p2;

    for (i = 0; i < A; i++)
        for (j = 0; j < C; j++)
        {
            v = 0;
            p1 = &m1.d[i * B];
            p2 = &m2.d[j];
            for (k = 0; k < B; k++, p1++, p2 += C)
                v += (*p1) * (*p2);
            *p++ = v;
        }

    return n;
}

template<class T, int A, int B>
    vec_c<T,A> operator* (const matrix_c<T,A,B> &m1, const vec_c<T,B> &v)
{
    vec_c<T,A>  n;
    int         i, k;
    T           sum, *p = n.d;
    const T     *p1;

    for (i = 0; i < A; i++)
    {
        sum = 0;
        p1 = &m1.d[i * B];
        for (k = 0; k < B; k++, p1++)
            sum += (*p1) * v.d[k];
        *p++ = sum;
    }

    return n;
}

template<class T, int A, int B>
    vec_c<T,B> operator* (const vec_c<T,A> &v, const matrix_c<T,A,B> &m1)
{
    vec_c<T,B>  n;
    int         i, k;
    T           sum, *p = n.d;
    const T     *p1;

    for (i = 0; i < B; i++)
    {
        sum = 0;
        p1 = &m1.d[i];
        for (k = 0; k < A; k++, p1 += B)
            sum += (*p1) * v.d[k];
        *p++ = sum;
    }

    return n;
}

/*
==================================================
mat2_c
==================================================
*/
template<class T> class mat2_c
{
public:
    T   d[4];

    // Kontstruktory
    mat2_c  () {};
    mat2_c  (T val) { d[0] = val; d[1] = val; d[2] = val; d[3] = val; }
    mat2_c  (T v1, T v2, T v3, T v4) { d[0] = v1; d[1] = v2; d[2] = v3; d[3] = v4; }
    mat2_c  (const matrix_c<T,2,2> &m) { set (m.d); }
    mat2_c  (const mat2_c &m) { *this = m; }

    // Funkce
    mat2_c&         identity    () { d[0] = (T) 1; d[1] = (T) 0; d[2] = (T) 0; d[3] = (T) 1; return *this; }
    matrix_c<T,2,2> mat         () const { matrix_c<T,2,2> m; memcpy (m.d, d, sizeof (d)); return m; }
    void            set         (T v1, T v2, T v3, T v4) { d[0] = v1; d[1] = v2; d[2] = v3; d[3] = v4; }
    void            set         (const T *v) { memcpy (d, v, sizeof (d)); }
    vec2_c<T>       row         (int n) const { if (!n) return vec2_c<T> (d[0], d[1]); return vec2_c<T> (d[2], d[3]); }
    vec2_c<T>       col         (int n) const { if (!n) return vec2_c<T> (d[0], d[2]); return vec2_c<T> (d[1], d[3]); }

    // Operatory
    mat2_c&     operator=       (const matrix_c<T,2,2> &m) { set (m.d); return *this; }
    mat2_c      operator+       (const mat2_c &m) const;
    mat2_c&     operator+=      (const mat2_c &m);
    mat2_c      operator-       () const;
    mat2_c      operator-       (const mat2_c &m) const;
    mat2_c&     operator-=      (const mat2_c &m);
    mat2_c      operator*       (T n) const;
    mat2_c&     operator*=      (T n);
    mat2_c      operator/       (T n) const;
    mat2_c&     operator/=      (T n);
    bool        operator==      (const mat2_c &m) const;
    bool        operator!=      (const mat2_c &m) const;

    mat2_c      operator*       (const mat2_c &m) const;
    vec2_c<T>   operator*       (const vec2_c<T> &v) const;

    T&          operator()      (int r, int s) { return d[(r << 1) + s]; }
    const T&    operator()      (int r, int s) const { return d[(r << 1) + s]; }
    mat2_c      operator~       () const;
};

template<class T>
    inline mat2_c<T> mat2_c<T>::operator+ (const mat2_c<T> &m) const
{
    return mat2_c<T> (d[0] + m.d[0], d[1] + m.d[1], d[2] + m.d[2], d[3] + m.d[3]);
}

template<class T>
    inline mat2_c<T>& mat2_c<T>::operator+= (const mat2_c<T> &m)
{
    d[0] += m.d[0]; d[1] += m.d[1]; d[2] += m.d[2]; d[3] += m.d[3];
    return *this;
}

template<class T>
    inline mat2_c<T> mat2_c<T>::operator- () const
{
    return mat2_c<T> (-d[0], -d[1], -d[2], -d[3]);
}

template<class T>
    inline mat2_c<T> mat2_c<T>::operator- (const mat2_c<T> &m) const
{
    return mat2_c<T> (d[0] - m.d[0], d[1] - m.d[1], d[2] - m.d[2], d[3] - m.d[3]);
}

template<class T>
    inline mat2_c<T>& mat2_c<T>::operator-= (const mat2_c<T> &m)
{
    d[0] -= m.d[0]; d[1] -= m.d[1]; d[2] -= m.d[2]; d[3] -= m.d[3];
    return *this;
}

template<class T>
    inline mat2_c<T> mat2_c<T>::operator* (T n) const
{
    return mat2_c<T> (d[0] * n, d[1] * n, d[2] * n, d[3] * n);
}

template<class T>
    inline mat2_c<T>& mat2_c<T>::operator*= (T n)
{
    d[0] *= n; d[1] *= n; d[2] *= n; d[3] *= n;
    return *this;
}

template<class T>
    inline mat2_c<T> mat2_c<T>::operator/ (T n) const
{
    return mat2_c<T> (d[0] / n, d[1] / n, d[2] / n, d[3] / n);
}

template<class T>
    inline mat2_c<T>& mat2_c<T>::operator/= (T n)
{
    d[0] /= n; d[1] /= n; d[2] /= n; d[3] /= n;
    return *this;
}

template<class T>
    inline bool mat2_c<T>::operator== (const mat2_c<T> &m) const
{
    return (d[0] == m.d[0] && d[1] == m.d[1] && d[2] == m.d[2] && d[3] == m.d[3]);
}

template<class T>
    inline bool mat2_c<T>::operator!= (const mat2_c<T> &m) const
{
    return (d[0] != m.d[0] || d[1] != m.d[1] || d[2] != m.d[2] || d[3] != m.d[3]);
}

template<class T>
    inline mat2_c<T> mat2_c<T>::operator~ () const
{
    return mat2_c<T> (d[0], d[2], d[1], d[3]);
}

template<class T>
    inline mat2_c<T> mat2_c<T>::operator* (const mat2_c<T> &m) const
{
    return mat2_c<T> (d[0] * m.d[0] + d[1] * m.d[2], d[0] * m.d[1] + d[1] * m.d[3],
                      d[2] * m.d[0] + d[3] * m.d[2], d[2] * m.d[1] + d[3] * m.d[3]);
}

template<class T>
    inline vec2_c<T> mat2_c<T>::operator* (const vec2_c<T> &v) const
{
    return vec2_c<T> (d[0] * v.x + d[1] * v.y, d[2] * v.x + d[3] * v.y);
}

template<class T>
    inline vec2_c<T> operator* (const vec2_c<T> &v, const mat2_c<T> &m)
{
    return vec2_c<T> (m.d[0] * v.x + m.d[1] * v.y, m.d[2] * v.x + m.d[3] * v.y);
}

template<class T>
    inline mat2_c<T> operator* (T n, const mat2_c<T> &m)
{
    return mat2_c<T> (m.d[0] * n, m.d[1] * n, m.d[2] * n, m.d[3] * n);
}

/*
==================================================
mat3_c
==================================================
*/
template<class T> class mat3_c
{
public:
    T   d[9];

    // Kontstruktory
    mat3_c  () {};
    mat3_c  (T val);
    mat3_c  (T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9);
    mat3_c  (const matrix_c<T,3,3> &m) { set (m.d); }
    mat3_c  (const mat3_c &m) { *this = m; }

    // Funkce
    mat3_c&         identity    ();
    matrix_c<T,3,3> mat         () const { matrix_c<T,3,3> m; memcpy (m.d, d, sizeof (d)); return m; }
    void            set         (T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9);
    void            set         (const T *v) { memcpy (d, v, sizeof (d)); }
    vec3_c<T>       row         (int n) const { n *= 3; return vec3_c<T> (d[n], d[n + 1], d[n + 2]); }
    vec3_c<T>       col         (int n) const { return vec3_c<T> (d[n], d[n + 3], d[n + 6]); }

    // Operatory
    mat3_c&     operator=       (const matrix_c<T,3,3> &m) { set (m.d); return *this; }
    mat3_c      operator+       (const mat3_c &m) const;
    mat3_c&     operator+=      (const mat3_c &m);
    mat3_c      operator-       () const;
    mat3_c      operator-       (const mat3_c &m) const;
    mat3_c&     operator-=      (const mat3_c &m);
    mat3_c      operator*       (T n) const;
    mat3_c&     operator*=      (T n);
    mat3_c      operator/       (T n) const;
    mat3_c&     operator/=      (T n);
    bool        operator==      (const mat3_c &m) const;
    bool        operator!=      (const mat3_c &m) const;

    mat3_c      operator*       (const mat3_c &m) const;
    vec3_c<T>   operator*       (const vec3_c<T> &v) const;

    T&          operator()      (int r, int s) { return d[3 * r + s]; }
    const T&    operator()      (int r, int s) const { return d[3 * r + s]; }
    mat3_c      operator~       () const;
};

template<class T>
    inline mat3_c<T>::mat3_c (T val)
{
    d[0] = val; d[1] = val; d[2] = val;
    d[3] = val; d[4] = val; d[5] = val;
    d[6] = val; d[7] = val; d[8] = val;
}

template<class T>
    inline mat3_c<T>::mat3_c (T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9)
{
    d[0] = v1; d[1] = v2; d[2] = v3;
    d[3] = v4; d[4] = v5; d[5] = v6;
    d[6] = v7; d[7] = v8; d[8] = v9;
}

template<class T>
    inline mat3_c<T>& mat3_c<T>::identity ()
{
    d[0] = (T) 1; d[1] = (T) 0; d[2] = (T) 0;
    d[3] = (T) 0; d[4] = (T) 1; d[5] = (T) 0;
    d[6] = (T) 0; d[7] = (T) 0; d[8] = (T) 1;
    return *this;
}

template<class T>
    inline void mat3_c<T>::set (T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9)
{
    d[0] = v1; d[1] = v2; d[2] = v3;
    d[3] = v4; d[4] = v5; d[5] = v6;
    d[6] = v7; d[7] = v8; d[8] = v9;
}

template<class T>
    inline mat3_c<T> mat3_c<T>::operator+ (const mat3_c<T> &m) const
{
    return mat3_c<T> (d[0] + m.d[0], d[1] + m.d[1], d[2] + m.d[2],
                      d[3] + m.d[3], d[4] + m.d[4], d[5] + m.d[5],
                      d[6] + m.d[6], d[7] + m.d[7], d[8] + m.d[8]);
}

template<class T>
    inline mat3_c<T>& mat3_c<T>::operator+= (const mat3_c<T> &m)
{
    d[0] += m.d[0]; d[1] += m.d[1]; d[2] += m.d[2];
    d[3] += m.d[3]; d[4] += m.d[4]; d[5] += m.d[5];
    d[6] += m.d[6]; d[7] += m.d[7]; d[8] += m.d[8];
    return *this;
}

template<class T>
    inline mat3_c<T> mat3_c<T>::operator- () const
{
    return mat3_c<T> (-d[0], -d[1], -d[2], -d[3], -d[4], -d[5], -d[6], -d[7], -d[8]);
}

template<class T>
    inline mat3_c<T> mat3_c<T>::operator- (const mat3_c<T> &m) const
{
    return mat3_c<T> (d[0] - m.d[0], d[1] - m.d[1], d[2] - m.d[2],
                      d[3] - m.d[3], d[4] - m.d[4], d[5] - m.d[5],
                      d[6] - m.d[6], d[7] - m.d[7], d[8] - m.d[8]);
}

template<class T>
    inline mat3_c<T>& mat3_c<T>::operator-= (const mat3_c<T> &m)
{
    d[0] -= m.d[0]; d[1] -= m.d[1]; d[2] -= m.d[2];
    d[3] -= m.d[3]; d[4] -= m.d[4]; d[5] -= m.d[5];
    d[6] -= m.d[6]; d[7] -= m.d[7]; d[8] -= m.d[8];
    return *this;
}

template<class T>
    inline mat3_c<T> mat3_c<T>::operator* (T n) const
{
    return mat3_c<T> (d[0] * n, d[1] * n, d[2] * n,
                      d[3] * n, d[4] * n, d[5] * n,
                      d[6] * n, d[7] * n, d[8] * n);
}

template<class T>
    inline mat3_c<T>& mat3_c<T>::operator*= (T n)
{
    d[0] *= n; d[1] *= n; d[2] *= n;
    d[3] *= n; d[4] *= n; d[5] *= n;
    d[6] *= n; d[7] *= n; d[8] *= n;
    return *this;
}

template<class T>
    inline mat3_c<T> mat3_c<T>::operator/ (T n) const
{
    return mat3_c<T> (d[0] / n, d[1] / n, d[2] / n,
                      d[3] / n, d[4] / n, d[5] / n,
                      d[6] / n, d[7] / n, d[8] / n);
}

template<class T>
    inline mat3_c<T>& mat3_c<T>::operator/= (T n)
{
    d[0] /= n; d[1] /= n; d[2] /= n;
    d[3] /= n; d[4] /= n; d[5] /= n;
    d[6] /= n; d[7] /= n; d[8] /= n;
    return *this;
}

template<class T>
    inline bool mat3_c<T>::operator== (const mat3_c<T> &m) const
{
    return (d[0] == m.d[0] && d[1] == m.d[1] && d[2] == m.d[2] &&
            d[3] == m.d[3] && d[4] == m.d[4] && d[5] == m.d[5] &&
            d[6] == m.d[6] && d[7] == m.d[7] && d[8] == m.d[8]);
}

template<class T>
    inline bool mat3_c<T>::operator!= (const mat3_c<T> &m) const
{
    return (d[0] != m.d[0] || d[1] != m.d[1] || d[2] != m.d[2] ||
            d[3] != m.d[3] || d[4] != m.d[4] || d[5] != m.d[5] ||
            d[5] != m.d[5] || d[6] != m.d[6] || d[7] != m.d[7]);
}

template<class T>
    inline mat3_c<T> mat3_c<T>::operator~ () const
{
    return mat3_c<T> (d[0], d[3], d[6], d[1], d[4], d[7], d[2], d[5], d[8]);
}

template<class T>
    inline mat3_c<T> mat3_c<T>::operator* (const mat3_c<T> &m) const
{
    return mat3_c<T> (d[0] * m.d[0] + d[1] * m.d[3] + d[2] * m.d[6],
                      d[0] * m.d[1] + d[1] * m.d[4] + d[2] * m.d[7],
                      d[0] * m.d[2] + d[1] * m.d[5] + d[2] * m.d[8],
                      d[3] * m.d[0] + d[4] * m.d[3] + d[5] * m.d[6],
                      d[3] * m.d[1] + d[4] * m.d[4] + d[5] * m.d[7],
                      d[3] * m.d[2] + d[4] * m.d[5] + d[5] * m.d[8],
                      d[6] * m.d[0] + d[7] * m.d[3] + d[8] * m.d[6],
                      d[6] * m.d[1] + d[7] * m.d[4] + d[8] * m.d[7],
                      d[6] * m.d[2] + d[7] * m.d[5] + d[8] * m.d[8]);
}

template<class T>
    inline vec3_c<T> mat3_c<T>::operator* (const vec3_c<T> &v) const
{
    return vec3_c<T> (d[0] * v.x + d[1] * v.y + d[2] * v.z,
                      d[3] * v.x + d[4] * v.y + d[5] * v.z,
                      d[6] * v.x + d[7] * v.y + d[8] * v.z);
}

template<class T>
    inline vec3_c<T> operator* (const vec3_c<T> &v, const mat3_c<T> &m)
{
    return vec3_c<T> (m.d[0] * v.x + m.d[1] * v.y + m.d[2] * v.z,
                      m.d[3] * v.x + m.d[4] * v.y + m.d[5] * v.z,
                      m.d[6] * v.x + m.d[7] * v.y + m.d[8] * v.z);
}

template<class T>
    inline mat3_c<T> operator* (T n, const mat3_c<T> &m)
{
    return mat3_c<T> (m.d[0] * n, m.d[1] * n, m.d[2] * n,
                      m.d[3] * n, m.d[4] * n, m.d[5] * n,
                      m.d[6] * n, m.d[7] * n, m.d[8] * n);
}

/*
==================================================
mat4_c
==================================================
*/
template<class T> class mat4_c
{
public:
    T   d[16];

    // Kontstruktory
    mat4_c  () {};
    mat4_c  (T val);
    mat4_c  (const matrix_c<T,4,4> &m) { set (m.d); }
    mat4_c  (const mat4_c &m) { *this = m; }

    // Funkce
    mat4_c&         identity    ();
    matrix_c<T,4,4> mat         () const { matrix_c<T,4,4> m; memcpy (m.d, d, sizeof (d)); return m; }
    void            set         (const T *v) { memcpy (d, v, sizeof (d)); }
    vec4_c<T>       row         (int n) const { n <<= 2; return vec4_c<T> (d[n], d[n + 1], d[n + 2], d[n + 3]); }
    vec4_c<T>       col         (int n) const { return vec4_c<T> (d[n], d[n + 4], d[n + 8], d[n + 12]); }

    // Operatory
    mat4_c&     operator=       (const matrix_c<T,4,4> &m) { set (m.d); return *this; }
    mat4_c      operator+       (const mat4_c &m) const;
    mat4_c&     operator+=      (const mat4_c &m);
    mat4_c      operator-       () const;
    mat4_c      operator-       (const mat4_c &m) const;
    mat4_c&     operator-=      (const mat4_c &m);
    mat4_c      operator*       (T n) const;
    mat4_c&     operator*=      (T n);
    mat4_c      operator/       (T n) const;
    mat4_c&     operator/=      (T n);
    bool        operator==      (const mat4_c &m) const;
    bool        operator!=      (const mat4_c &m) const;

    mat4_c      operator*       (const mat4_c &m) const;
    vec4_c<T>   operator*       (const vec4_c<T> &v) const;

    T&          operator()      (int r, int s) { return d[(r << 2) + s]; }
    const T&    operator()      (int r, int s) const { return d[(r << 2) + s]; }
    mat4_c      operator~       () const;
};

template<class T>
    inline mat4_c<T>::mat4_c (T val)
{
    for (int i = 0; i < 16; i++)
        d[i] = val;
}

template<class T>
    inline mat4_c<T>& mat4_c<T>::identity ()
{
    d[0] = (T) 1; d[1] = (T) 0; d[2] = (T) 0; d[3] = (T) 0;
    d[4] = (T) 1; d[5] = (T) 1; d[6] = (T) 0; d[7] = (T) 0;
    d[8] = (T) 1; d[9] = (T) 0; d[10] = (T) 1; d[11] = (T) 0;
    d[12] = (T) 1; d[13] = (T) 0; d[14] = (T) 0; d[15] = (T) 1;
    return *this;
}

template<class T>
    inline mat4_c<T> mat4_c<T>::operator+ (const mat4_c<T> &m) const
{
    mat4_c<T>   n;
    for (int i = 0; i < 16; i++)
        n.d[i] = d[i] + m.d[i];
    return n;
}

template<class T>
    inline mat4_c<T>& mat4_c<T>::operator+= (const mat4_c<T> &m)
{
    for (int i = 0; i < 16; i++)
        d[i] += m.d[i];
    return *this;
}

template<class T>
    inline mat4_c<T> mat4_c<T>::operator- () const
{
    mat4_c<T>   n;
    for (int i = 0; i < 16; i++)
        n.d[i] = -d[i];
    return n;
}

template<class T>
    inline mat4_c<T> mat4_c<T>::operator- (const mat4_c<T> &m) const
{
    mat4_c<T>   n;
    for (int i = 0; i < 16; i++)
        n.d[i] = d[i] - m.d[i];
    return n;
}

template<class T>
    inline mat4_c<T>& mat4_c<T>::operator-= (const mat4_c<T> &m)
{
    for (int i = 0; i < 16; i++)
        d[i] -= m.d[i];
    return *this;
}

template<class T>
    inline mat4_c<T> mat4_c<T>::operator* (T n) const
{
    mat4_c<T>   a;
    for (int i = 0; i < 16; i++)
        a.d[i] = d[i] * n;
    return a;
}

template<class T>
    inline mat4_c<T>& mat4_c<T>::operator*= (T n)
{
    for (int i = 0; i < 16; i++)
        d[i] *= n;
    return *this;
}

template<class T>
    inline mat4_c<T> mat4_c<T>::operator/ (T n) const
{
    mat4_c<T>   a;
    for (int i = 0; i < 16; i++)
        a.d[i] = d[i] / n;
    return a;
}

template<class T>
    inline mat4_c<T>& mat4_c<T>::operator/= (T n)
{
    for (int i = 0; i < 16; i++)
        d[i] /= n;
    return *this;
}

template<class T>
    inline bool mat4_c<T>::operator== (const mat4_c<T> &m) const
{
    for (int i = 0; i < 16; i++)
        if (d[i] != m.d[i])
            return false;
    return true;
}

template<class T>
    inline bool mat4_c<T>::operator!= (const mat4_c<T> &m) const
{
    for (int i = 0; i < 16; i++)
        if (d[i] != m.d[i])
            return true;
    return false;
}

template<class T>
    inline mat4_c<T> mat4_c<T>::operator~ () const
{
    mat4_c<T>   n;
    n.d[0] = d[0]; n.d[1] = d[4]; n.d[2] = d[8]; n.d[3] = d[12];
    n.d[4] = d[1]; n.d[5] = d[5]; n.d[6] = d[9]; n.d[7] = d[13];
    n.d[8] = d[2]; n.d[9] = d[6]; n.d[10] = d[10]; n.d[11] = d[14];
    n.d[12] = d[3]; n.d[13] = d[7]; n.d[14] = d[11]; n.d[15] = d[15];
    return n;
}

template<class T>
    inline mat4_c<T> mat4_c<T>::operator* (const mat4_c<T> &m) const
{
    mat4_c<T>   n;
    n.d[0] = d[0] * m.d[0] + d[1] * m.d[4] + d[2] * m.d[8] + d[3] * m.d[12];
    n.d[1] = d[4] * m.d[0] + d[5] * m.d[4] + d[6] * m.d[8] + d[7] * m.d[12];
    n.d[2] = d[8] * m.d[0] + d[9] * m.d[4] + d[10] * m.d[8] + d[11] * m.d[12];
    n.d[3] = d[12] * m.d[0] + d[13] * m.d[4] + d[14] * m.d[8] + d[15] * m.d[12];
    n.d[4] = d[0] * m.d[1] + d[1] * m.d[5] + d[2] * m.d[9] + d[3] * m.d[13];
    n.d[5] = d[4] * m.d[1] + d[5] * m.d[5] + d[6] * m.d[9] + d[7] * m.d[13];
    n.d[6] = d[8] * m.d[1] + d[9] * m.d[5] + d[10] * m.d[9] + d[11] * m.d[13];
    n.d[7] = d[12] * m.d[1] + d[13] * m.d[5] + d[14] * m.d[9] + d[15] * m.d[13];
    n.d[8] = d[0] * m.d[2] + d[1] * m.d[6] + d[2] * m.d[10] + d[3] * m.d[14];
    n.d[9] = d[4] * m.d[2] + d[5] * m.d[6] + d[6] * m.d[10] + d[7] * m.d[14];
    n.d[10] = d[8] * m.d[2] + d[9] * m.d[6] + d[10] * m.d[10] + d[11] * m.d[14];
    n.d[11] = d[12] * m.d[2] + d[13] * m.d[6] + d[14] * m.d[10] + d[15] * m.d[14];
    n.d[12] = d[0] * m.d[3] + d[1] * m.d[7] + d[2] * m.d[11] + d[3] * m.d[15];
    n.d[13] = d[4] * m.d[3] + d[5] * m.d[7] + d[6] * m.d[11] + d[7] * m.d[15];
    n.d[14] = d[8] * m.d[3] + d[9] * m.d[7] + d[10] * m.d[11] + d[11] * m.d[15];
    n.d[15] = d[12] * m.d[3] + d[13] * m.d[7] + d[14] * m.d[11] + d[15] * m.d[15];
    return n;
}

template<class T>
    inline vec4_c<T> mat4_c<T>::operator* (const vec4_c<T> &v) const
{
    vec4_c<T>   n;
    n.x = d[0] * v.x + d[1] * v.y + d[2] * v.z + d[3] * v.w;
    n.y = d[4] * v.x + d[5] * v.y + d[6] * v.z + d[7] * v.w;
    n.z = d[8] * v.x + d[9] * v.y + d[10] * v.z + d[11] * v.w;
    n.w = d[12] * v.x + d[13] * v.y + d[14] * v.z + d[15] * v.w;
    return n;
}

template<class T>
    inline vec4_c<T> operator* (const vec4_c<T> &v, const mat4_c<T> &m)
{
    vec4_c<T>   n;
    n.x = m.d[0] * v.x + m.d[4] * v.y + m.d[8] * v.z + m.d[12] * v.w;
    n.y = m.d[1] * v.x + m.d[5] * v.y + m.d[9] * v.z + m.d[13] * v.w;
    n.z = m.d[2] * v.x + m.d[6] * v.y + m.d[10] * v.z + m.d[14] * v.w;
    n.w = m.d[3] * v.x + m.d[7] * v.y + m.d[11] * v.z + m.d[15] * v.w;
    return n;
}

template<class T>
    inline mat4_c<T> operator* (T n, const mat4_c<T> &m)
{
    mat4_c<T>   a;
    for (int i = 0; i < 16; i++)
        a.d[i] = m.d[i] * n;
    return a;
}

/*
==================================

    ROVINY

==================================
*/
// Pro roviny plati: x * Normal[0] + y * Normal[1] + z * Normal[2] = Dist

/*
==================================================
plane3_c
==================================================
*/
template<class T>
class plane3_c
{
public:
    vec3_c<T>   m_normal;
    T           m_dist;

    plane3_c    () {};
    plane3_c    (const plane3_c &p) { *this = p; }
    plane3_c    (const vec3_c<T> &p1, const vec3_c<T> &p2, const vec3_c<T> &p3) { get (p1, p2, p3); }

    void    calcdist    (const vec3_c<T> &point) { m_dist = m_normal * point; }
    mval_t  distfrom    (const vec3_c<T> &point) const;
    void    get         (const vec3_c<T> &p1, const vec3_c<T> &p2, const vec3_c<T> &p3);
    bool    isabove     (const vec3_c<T> &point) const;
};

template<class T>
inline mval_t plane3_c<T>::distfrom (const vec3_c<T> &point) const
{
    return ((m_normal * point) - m_dist) / m_normal.len ();
}

template<class T>
inline void plane3_c<T>::get (const vec3_c<T> &p1, const vec3_c<T> &p2, const vec3_c<T> &p3)
{
    m_normal = (p2 - p1) % (p3 - p2);
    m_dist = m_normal * p1;
}

template<class T>
inline bool plane3_c<T>::isabove (const vec3_c<T> &point) const
{
    return m_normal * point >= m_dist;
}

/*
==================================

    BOUNDING BOX

==================================
*/
template<class T> class bbox3_c
{
public:
    vec3_c<T>   corner[8];

    void    centerdist  (const vec3_c<T> &c, const vec3_c<T> &d);
    void    minmax      (const vec3_c<T> &min, const vec3_c<T> &max);
};

template<class T>
    inline void bbox3_c<T>::centerdist (const vec3_c<T> &c, const vec3_c<T> &d)
{
    corner[0] = c - d;
    corner[1].set (c.x - d.x, c.y - d.y, c.z + d.z);
    corner[2].set (c.x + d.x, c.y - d.y, c.z + d.z);
    corner[3].set (c.x + d.x, c.y - d.y, c.z - d.z);
    corner[4].set (c.x - d.x, c.y + d.y, c.z - d.z);
    corner[5].set (c.x - d.x, c.y + d.y, c.z + d.z);
    corner[6] = c + d;
    corner[7].set (c.x + d.x, c.y + d.y, c.z - d.z);
}

template<class T>
    inline void bbox3_c<T>::minmax (const vec3_c<T> &min, const vec3_c<T> &max)
{
    corner[0].Set (min.x, min.y, min.z);
    corner[1].Set (min.x, min.y, max.z);
    corner[2].Set (max.x, min.y, max.z);
    corner[3].Set (max.x, min.y, min.z);
    corner[4].Set (min.x, max.y, min.z);
    corner[5].Set (min.x, max.y, max.z);
    corner[6].Set (max.x, max.y, max.z);
    corner[7].Set (max.x, max.y, min.z);
}

#endif
