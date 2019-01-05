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

#include <vector>
#include "GL4Buffer.h"
#include "../../Vertex.h"
#include "../../FaceList.h"

namespace Duel6 {
    GL4Buffer::GL4Buffer(GL4Program &program, const FaceList &faceList)
            : program(program), elements(6 * faceList.getFaces().size()) {
        std::vector<Vertex> vertexBuffer;
        createFaceListVertexBuffer(faceList, vertexBuffer);

        std::vector<Float32> textureIndexBuffer;
        createFaceListTextureIndexBuffer(faceList, textureIndexBuffer);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vertexVbo);
        glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
        glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex), vertexBuffer.data(), GL_STATIC_DRAW);
        //glNamedBufferStorage(vertexVbo, vertexBuffer.size() * sizeof(Float32), vertexBuffer.data(), 0); // GL 4.5

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) (3 * sizeof(Float32)));
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const GLvoid *) (5 * sizeof(Float32)));
        glEnableVertexAttribArray(3);

        glGenBuffers(1, &textureIndexVbo);
        glBindBuffer(GL_ARRAY_BUFFER, textureIndexVbo);
        glBufferData(GL_ARRAY_BUFFER, textureIndexBuffer.size() * sizeof(Float32), textureIndexBuffer.data(),
                     GL_STATIC_DRAW);
        //glNamedBufferStorage(textureIndexVbo, textureIndexBuffer.size() * sizeof(Float32), textureIndexBuffer.data(), 0); // GL 4.5

        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    GL4Buffer::~GL4Buffer() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vertexVbo);
        glDeleteBuffers(1, &textureIndexVbo);
    }

    void GL4Buffer::update(const FaceList &faceList) {
        std::vector<Float32> textureIndexBuffer;
        createFaceListTextureIndexBuffer(faceList, textureIndexBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, textureIndexVbo);
        glBufferData(GL_ARRAY_BUFFER, textureIndexBuffer.size() * sizeof(Float32), textureIndexBuffer.data(),
                     GL_STATIC_DRAW);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, elements * sizeof(Float32), textureIndexBuffer.data());
        // glNamedBufferSubData(textureIndexVbo, 0, elements * sizeof(Float32), textureIndexBuffer.data()); // GL 4.5
    }

    void GL4Buffer::render(const Material &material) {
        glBindVertexArray(vao);
        program.bind();

        glBindTexture(GL_TEXTURE_2D_ARRAY, material.getTexture());
        program.setUniform("alphaTest", material.isMasked() ? 1 : 0);

        const Color &color = material.getColor();
        Float32 colorData[4] = {color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f,
                                color.getAlpha() / 255.0f};
        program.setUniform("color", colorData);

        glDrawArrays(GL_TRIANGLES, 0, elements);
    }


    void GL4Buffer::createFaceListVertexBuffer(const FaceList &faceList, std::vector<Vertex> &vertexBuffer) {
        const Vertex *vertex = faceList.getVertexes().data();
        const auto &faces = faceList.getFaces();

        const auto faceCount = faces.size();
        vertexBuffer.reserve(faceCount * 6);

        for (Size i = 0; i < faceCount; i++, vertex += 4) {
            const Vertex &v1 = vertex[0];
            const Vertex &v2 = vertex[1];
            const Vertex &v3 = vertex[2];
            const Vertex &v4 = vertex[3];

            vertexBuffer.push_back(v1);
            vertexBuffer.push_back(v2);
            vertexBuffer.push_back(v3);
            vertexBuffer.push_back(v1);
            vertexBuffer.push_back(v3);
            vertexBuffer.push_back(v4);
        }
    }

    void
    GL4Buffer::createFaceListTextureIndexBuffer(const FaceList &faceList, std::vector<Float32> &textureIndexBuffer) {
        const auto &faces = faceList.getFaces();
        textureIndexBuffer.reserve(faces.size() * 6);

        for (const Face &face : faces) {
            Float32 textureIndex = face.getCurrentTexture();
            textureIndexBuffer.push_back(textureIndex);
            textureIndexBuffer.push_back(textureIndex);
            textureIndexBuffer.push_back(textureIndex);
            textureIndexBuffer.push_back(textureIndex);
            textureIndexBuffer.push_back(textureIndex);
            textureIndexBuffer.push_back(textureIndex);
        }
    }
}