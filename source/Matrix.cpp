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

#include <cstring>
#include "Math.h"
#include "Matrix.h"

namespace Duel6 {
    const Matrix Matrix::IDENTITY = Matrix::scale(1, 1, 1);

    Matrix::Matrix() {}

    Matrix::Matrix(const Matrix &m) {
        *this = m;
    }

    Matrix &Matrix::operator=(const Matrix &m) {
        memcpy(data, m.data, 16 * sizeof(Float32));
        return *this;
    }

    Float32 &Matrix::operator()(Int32 column, Int32 row) {
        return data[row + 4 * column];
    }

    Float32 Matrix::operator()(Int32 column, Int32 row) const {
        return data[row + 4 * column];
    }

    Vector Matrix::getColumn(Int32 index) const {
        const Float32 *columnPtr = &data[index * 4];
        return Vector(columnPtr[0], columnPtr[1], columnPtr[2]);
    }

    Matrix &Matrix::setColumn(Int32 index, Vector column) {
        Float32 *columnPtr = &data[index * 4];
        columnPtr[0] = column.x;
        columnPtr[1] = column.y;
        columnPtr[2] = column.z;
        return *this;
    }

//    Vector Matrix::getRow(Int32 index) const
//    {}
//
//    Matrix& Matrix::setRow(Int32 index, Vector row)
//    {}

    Matrix Matrix::operator+(const Matrix &m) const {
        Matrix result;
        Float32 *dest = result.data;
        const Float32 *left = data;
        const Float32 *right = m.data;

        for (Int32 i = 0; i < 16; i++, dest++, left++, right++) {
            *dest = *left + *right;
        }

        return result;
    }

    Matrix &Matrix::operator+=(const Matrix &m) {
        Float32 *dest = data;
        const Float32 *src = m.data;

        for (Int32 i = 0; i < 16; i++, dest++, src++) {
            *dest += *src;
        }

        return *this;
    }

    Matrix Matrix::operator-(const Matrix &m) const {
        Matrix result;
        Float32 *dest = result.data;
        const Float32 *left = data;
        const Float32 *right = m.data;

        for (Int32 i = 0; i < 16; i++, dest++, left++, right++) {
            *dest = *left - *right;
        }

        return result;
    }

    Matrix &Matrix::operator-=(const Matrix &m) {
        Float32 *dest = data;
        const Float32 *src = m.data;

        for (Int32 i = 0; i < 16; i++, dest++, src++) {
            *dest += *src;
        }

        return *this;
    }

    Matrix Matrix::operator*(const Matrix &m) const {
        Matrix result;
        Float32 *dest = result.data;

        for (Int32 column = 0; column < 4; column++) {
            for (Int32 row = 0; row < 4; row++, dest++) {
                Float32 sum = 0;
                const Float32 *left = &data[row];
                const Float32 *right = &m.data[4 * column];

                for (Int32 i = 0; i < 4; i++, left += 4, right++) {
                    sum += (*left) * (*right);
                }

                *dest = sum;
            }
        }

        return result;
    }

    Matrix Matrix::operator*(Float32 value) const {
        Matrix result;
        Float32 *dest = result.data;
        const Float32 *src = data;

        for (Int32 i = 0; i < 16; i++, dest++, src++) {
            *dest = *src * value;
        }

        return result;
    }

    Matrix &Matrix::operator*=(Float32 value) {
        Float32 *ptr = data;

        for (Int32 i = 0; i < 16; i++, ptr++) {
            *ptr *= value;
        }

        return *this;
    }

    Matrix Matrix::operator/(Float32 value) const {
        Matrix result;
        Float32 *dest = result.data;
        const Float32 *src = data;

        for (Int32 i = 0; i < 16; i++, dest++, src++) {
            *dest = *src / value;
        }

        return result;
    }

    Matrix &Matrix::operator/=(Float32 value) {
        Float32 *ptr = data;

        for (Int32 i = 0; i < 16; i++, ptr++) {
            *ptr /= value;
        }

        return *this;
    }

    Vector Matrix::operator*(const Vector &v) const {
        Float32 x = data[0] * v.x + data[4] * v.y + data[8] * v.z + data[12];
        Float32 y = data[1] * v.x + data[5] * v.y + data[9] * v.z + data[13];
        Float32 z = data[2] * v.x + data[6] * v.y + data[10] * v.z + data[14];
        Float32 w = data[3] * v.x + data[7] * v.y + data[11] * v.z + data[15];
        return Vector(x / w, y / w, z / w);
    }

    Matrix Matrix::translate(Float32 x, Float32 y, Float32 z) {
        Matrix result;

        Float32 *dest = result.data;
        memset(dest, 0, 16 * sizeof(Float32));
        dest[0] = 1;
        dest[5] = 1;
        dest[10] = 1;
        dest[15] = 1;
        dest[12] = x;
        dest[13] = y;
        dest[14] = z;

        return result;
    }

    Matrix Matrix::scale(Float32 x, Float32 y, Float32 z) {
        Matrix result;

        Float32 *dest = result.data;
        memset(dest, 0, 16 * sizeof(Float32));
        dest[0] = x;
        dest[5] = y;
        dest[10] = z;
        dest[15] = 1;

        return result;
    }

    Matrix Matrix::rotate(Float32 radians, const Vector &axis) {
        Matrix result;
        Float32 *dest = result.data;

        Float32 c = Math::radianCos(radians);
        Float32 mc = 1 - c;
        Float32 s = Math::radianSin(radians);

        Float32 x = axis.x;
        Float32 y = axis.y;
        Float32 z = axis.z;

        dest[0] = x * x * mc + c;
        dest[1] = x * y * mc + z * s;
        dest[2] = x * z * mc - y * s;
        dest[3] = 0;

        dest[4] = x * y * mc - z * s;
        dest[5] = y * y * mc + c;
        dest[6] = y * z * mc + x * s;
        dest[7] = 0;

        dest[8] = x * z * mc + y * s;
        dest[9] = y * z * mc - x * s;
        dest[10] = z * z * mc + c;
        dest[11] = 0;

        dest[12] = 0;
        dest[13] = 0;
        dest[14] = 0;
        dest[15] = 1;

        return result;
    }

    // Source: gluPerspective
    Matrix Matrix::perspective(Float32 fovAngle, Float32 aspect, Float32 nearClip, Float32 farClip) {
        Float32 fovY = Math::angleToRadians(fovAngle) / 2;
        Float32 f = Math::radianCos(fovY) / Math::radianSin(fovY);

        Matrix result;
        Float32 *dest = result.data;

        dest[0] = f / aspect;
        dest[1] = 0;
        dest[2] = 0;
        dest[3] = 0;

        dest[4] = 0;
        dest[5] = f;
        dest[6] = 0;
        dest[7] = 0;

        dest[8] = 0;
        dest[9] = 0;
        dest[10] = (nearClip + farClip) / (nearClip - farClip);
        dest[11] = -1;

        dest[12] = 0;
        dest[13] = 0;
        dest[14] = (2 * nearClip * farClip) / (nearClip - farClip);
        dest[15] = 0;

        return result;
    }

    // Source: glOrtho
    Matrix Matrix::orthographic(Float32 left, Float32 right, Float32 bottom, Float32 top, Float32 near, Float32 far) {
        Matrix result;
        Float32 *dest = result.data;

        dest[0] = 2 / (right - left);
        dest[1] = 0;
        dest[2] = 0;
        dest[3] = 0;

        dest[4] = 0;
        dest[5] = 2 / (top - bottom);
        dest[6] = 0;
        dest[7] = 0;

        dest[8] = 0;
        dest[9] = 0;
        dest[10] = 2 / (near - far);
        dest[11] = 0;

        dest[12] = (right + left) / (left - right);
        dest[13] = (top + bottom) / (bottom - top);
        dest[14] = (near + far) / (near - far);
        dest[15] = 1;

        return result;
    }

    // Source: gluLookAt
    Matrix Matrix::lookAt(const Vector &eye, const Vector &front, const Vector &up) {
        Vector s = front.cross(up);
        Vector u = s.cross(front);

        Matrix result;
        Float32 *dest = result.data;

        dest[0] = s.x;
        dest[4] = s.y;
        dest[8] = s.z;
        dest[12] = 0.0f;

        dest[1] = u.x;
        dest[5] = u.y;
        dest[9] = u.z;
        dest[13] = 0.0f;

        dest[2] = -front.x;
        dest[6] = -front.y;
        dest[10] = -front.z;
        dest[14] = 0.0f;

        dest[3] = 0.0f;
        dest[7] = 0.0f;
        dest[11] = 0.0f;
        dest[15] = 1.0f;

        return result * translate(-eye);
    }

    Vector operator*(const Vector &v, const Matrix &m) {
        const Float32 *data = m.getStorage();
        Float32 x = data[0] * v.x + data[1] * v.y + data[2] * v.z + data[3];
        Float32 y = data[4] * v.x + data[5] * v.y + data[6] * v.z + data[7];
        Float32 z = data[8] * v.x + data[9] * v.y + data[10] * v.z + data[11];
        Float32 w = data[12] * v.x + data[13] * v.y + data[14] * v.z + data[15];
        return Vector(x / w, y / w, z / w);
    }
}