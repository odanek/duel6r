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

#include "FaceList.h"
#include "Video.h"

namespace Duel6 {
    void FaceList::optimize() {
        for (Size i = 0; i < faces.size(); i++) {
            Uint32 curTexture = faces[i].getCurrentTexture();
            Size curFace = i + 1;

            for (Size j = i + 1; j < faces.size(); j++) {
                if (faces[j].getCurrentTexture() == curTexture && j != curFace) {
                    std::swap(faces[curFace], faces[j]);

                    for (Size k = 0; k < 4; k++) {
                        std::swap(vertexes[curFace * 4 + k], vertexes[j * 4 + k]);
                    }

                    curFace++;
                }
            }
        }
    }

    void FaceList::render(const TextureList &textures, bool masked) const {
        if (faces.empty()) {
            return;
        }

        const Vertex *vertex = &vertexes[0];

        for (const Face &face : faces) {
            const Texture &texture = textures.at(face.getCurrentTexture());
            Material material(texture, Color::WHITE, masked);

            const Vertex &v1 = vertex[0];
            const Vertex &v2 = vertex[1];
            const Vertex &v3 = vertex[2];
            const Vertex &v4 = vertex[3];

            globRenderer->triangle(Vector(v1.x, v1.y, v1.z), Vector(v1.u, v1.v),
                                   Vector(v2.x, v2.y, v2.z), Vector(v2.u, v2.v),
                                   Vector(v3.x, v3.y, v3.z), Vector(v3.u, v3.v), material);

            globRenderer->triangle(Vector(v1.x, v1.y, v1.z), Vector(v1.u, v1.v),
                                   Vector(v3.x, v3.y, v3.z), Vector(v3.u, v3.v),
                                   Vector(v4.x, v4.y, v4.z), Vector(v4.u, v4.v), material);

            vertex += 4;
        }
    }

    void FaceList::nextFrame() {
        for (Face &face : faces) {
            face.nextFrame();
        }
    }
}
