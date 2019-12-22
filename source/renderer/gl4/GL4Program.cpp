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
#include "../../Exception.h"
#include "../../VideoException.h"
#include "GL4Program.h"

namespace Duel6 {
    GL4Program::GL4Program(const GL4Shader &vertexShader, const GL4Shader &fragmentShader)
            : vertexShader(vertexShader), fragmentShader(fragmentShader) {
        id = glCreateProgram();

        glAttachShader(id, vertexShader.getId());
        glAttachShader(id, fragmentShader.getId());

        glLinkProgram(id);

        GLint isLinked = 0;
        glGetProgramiv(id, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(id, maxLength, &maxLength, infoLog.data());

            glDeleteProgram(id);

            D6_THROW(VideoException, (const char *)infoLog.data());
        }
    }

    GL4Program::~GL4Program() {
        if (id != 0) {
            glDeleteProgram(id);
            id = 0;
        }
    }

    void GL4Program::bind() {
        glUseProgram(id);
    }

    GLint GL4Program::uniformLocation(const GLchar *name) {
        return glGetUniformLocation(id, name);
    }

    void GL4Program::setUniform(const GLchar *name, Int32 value) {
        glProgramUniform1i(id, uniformLocation(name), value);
    }

    void GL4Program::setUniform(const GLchar *name, Float32 value) {
        glProgramUniform1f(id, uniformLocation(name), value);
    }

    void GL4Program::setUniform(const GLchar *name, Float32 value[4]) {
        glProgramUniform4fv(id, uniformLocation(name), 1, value);
    }

    void GL4Program::setUniform(const GLchar *name, const Matrix &value) {
        glProgramUniformMatrix4fv(id, uniformLocation(name), 1, GL_FALSE, value.getStorage());
    }

    GLuint GL4Program::getId() const {
        return id;
    }

    const GL4Shader &GL4Program::getVertexShader() const {
        return vertexShader;
    }

    const GL4Shader &GL4Program::getFragmentShader() const {
        return fragmentShader;
    }
}