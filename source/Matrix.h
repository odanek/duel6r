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

#ifndef DUEL6_MATRIX_H
#define DUEL6_MATRIX_H

#include "Type.h"
#include "Vector.h"

namespace Duel6 {
    class Matrix {
    private:
        Float32 data[16];

    public:
        static const Matrix IDENTITY;

    public:
        Matrix();

        Matrix(const Matrix &m);

        Matrix &operator=(const Matrix &m);

        Float32 *getStorage() {
            return data;
        }

        const Float32 *getStorage() const {
            return data;
        }

        Float32 &operator()(Int32 column, Int32 row);

        Float32 operator()(Int32 column, Int32 row) const;

        Vector getColumn(Int32 index) const;

        Matrix &setColumn(Int32 index, Vector column);
//        Vector getRow(Int32 index) const;
//        Matrix& setRow(Int32 index, Vector row);

        Matrix operator+(const Matrix &m) const;

        Matrix &operator+=(const Matrix &m);

        Matrix operator-(const Matrix &m) const;

        Matrix &operator-=(const Matrix &m);

        Matrix operator*(const Matrix &m) const;

        Matrix operator*(Float32 value) const;

        Matrix &operator*=(Float32 value);

        Matrix operator/(Float32 value) const;

        Matrix &operator/=(Float32 value);

        Vector operator*(const Vector &v) const;

        static Matrix translate(Float32 x, Float32 y, Float32 z);

        static Matrix translate(const Vector &v) {
            return translate(v.x, v.y, v.z);
        }

        static Matrix scale(Float32 x, Float32 y, Float32 z);

        static Matrix scale(const Vector &v) {
            return scale(v.x, v.y, v.z);
        }

        static Matrix rotate(Float32 radians, const Vector &axis);

        static Matrix perspective(Float32 fov, Float32 aspect, Float32 nearClip, Float32 farClip);

        static Matrix orthographic(Float32 left, Float32 right, Float32 bottom, Float32 top, Float32 near, Float32 far);

        static Matrix lookAt(const Vector &eye, const Vector &front, const Vector &up);
    };

    Vector operator*(const Vector &v, const Matrix &m);
}

#endif