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

#ifndef DUEL6_FILE_H
#define DUEL6_FILE_H

#include <stdio.h>
#include <string>
#include <vector>
#include "Type.h"

namespace Duel6 {
    class File {
    public:
        enum class Seek {
            Set,
            Cur,
            End
        };

        enum class Mode {
            Binary,
            Text
        };

        enum class Access {
            Read,
            Write,
            ReadWrite
        };

    private:
        FILE *handle;

    public:
        File(const std::string &path, Mode mode, Access access);

        ~File() {
            close();
        }

        File &open(const std::string &path, Mode mode, Access access);

        File &close();

        File &read(void *ptr, Size size, Size count);

        File &write(const void *ptr, Size size, Size count);

        File &seek(long offset, Seek seek);

        bool isEof() const;

        static Size getSize(const std::string &path);

        static bool exists(const std::string &path);

        static void load(const std::string &path, long offset, void *ptr);

        static void
        listDirectory(const std::string &path, const std::string &extension, std::vector<std::string> &fileNames);

        static Size countFiles(const std::string &path, const std::string &extension);
    };
}

#endif
