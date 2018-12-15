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

#include "RendererBase.h"

namespace Duel6 {
    RendererBase::RendererBase()
            : projectionMatrix(Matrix::IDENTITY), viewMatrix(Matrix::IDENTITY), modelMatrix(Matrix::IDENTITY) {}

    void RendererBase::setProjectionMatrix(const Matrix &m) {
        projectionMatrix = m;
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    }

    Matrix RendererBase::getProjectionMatrix() const {
        return projectionMatrix;
    }

    void RendererBase::setViewMatrix(const Matrix &m) {
        viewMatrix = m;
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    }

    Matrix RendererBase::getViewMatrix() const {
        return viewMatrix;
    }

    void RendererBase::setModelMatrix(const Matrix &m) {
        modelMatrix = m;
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    }

    Matrix RendererBase::getModelMatrix() const {
        return modelMatrix;
    }
    
    void RendererBase::quadXY(const Vector &position, const Vector &size, const Color &color) {
        Vector p2(position.x, position.y + size.y, position.z);
        Vector p3(position.x + size.x, position.y + size.y, position.z);
        Vector p4(position.x + size.x, position.y, position.z);
        quad(position, p2, p3, p4, color);
    }

    void RendererBase::quadXY(const Vector &position, const Vector &size,
                             const Vector &texturePosition, const Vector &textureSize,
                             const Material &material) {
        Vector p2(position.x, position.y + size.y, position.z);
        Vector t2(texturePosition.x, texturePosition.y + textureSize.y, texturePosition.z);
        Vector p3(position.x + size.x, position.y + size.y, position.z);
        Vector t3(texturePosition.x + textureSize.x, texturePosition.y + textureSize.y, texturePosition.z);
        Vector p4(position.x + size.x, position.y, position.z);
        Vector t4(texturePosition.x + textureSize.x, texturePosition.y, texturePosition.z);
        quad(position, texturePosition, p2, t2, p3, t3, p4, t4, material);
    }

    void RendererBase::quadXZ(const Vector &position, const Vector &size, const Color &color) {
        Vector p2(position.x + size.x, position.y, position.z);
        Vector p3(position.x + size.x, position.y, position.z + size.z);
        Vector p4(position.x, position.y, position.z + size.z);
        quad(position, p2, p3, p4, color);
    }

    void RendererBase::quadXZ(const Vector &position, const Vector &size,
                             const Vector &texturePosition, const Vector &textureSize,
                             const Material &material) {
        Vector p2(position.x + size.x, position.y, position.z);
        Vector t2(texturePosition.x + textureSize.x, texturePosition.y);
        Vector p3(position.x + size.x, position.y, position.z + size.z);
        Vector t3(texturePosition.x + textureSize.x, texturePosition.y + textureSize.y);
        Vector p4(position.x, position.y, position.z + size.z);
        Vector t4(texturePosition.x, texturePosition.y + textureSize.y);
        quad(position, texturePosition, p2, t2, p3, t3, p4, t4, material);
    }

    void RendererBase::quadYZ(const Vector &position, const Vector &size, const Color &color) {
        Vector p2(position.x, position.y + size.y, position.z);
        Vector p3(position.x, position.y + size.y, position.z + size.z);
        Vector p4(position.x, position.y, position.z + size.z);
        quad(position, p2, p3, p4, color);
    }

    void RendererBase::quadYZ(const Vector &position, const Vector &size,
                             const Vector &texturePosition, const Vector &textureSize,
                             const Material &material) {
        Vector p2(position.x, position.y + size.y, position.z);
        Vector t2(texturePosition.x, texturePosition.y + textureSize.y);
        Vector p3(position.x, position.y + size.y, position.z + size.z);
        Vector t3(texturePosition.x + textureSize.x, texturePosition.y + textureSize.y);
        Vector p4(position.x, position.y, position.z + size.z);
        Vector t4(texturePosition.x + textureSize.x, texturePosition.y);
        quad(position, texturePosition, p2, t2, p3, t3, p4, t4, material);
    }

    void RendererBase::frame(const Vector &position, const Vector &size, Float32 width, const Color &color) {
        Vector p2(position.x, position.y + size.y);
        Vector p3(position.x + size.x, position.y + size.y);
        Vector p4(position.x + size.x, position.y);

        line(position, p2, width, color);
        line(p2, p3, width, color);
        line(p3, p4, width, color);
        line(p4, position, width, color);
    }
}
