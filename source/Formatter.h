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

#ifndef DUEL6_FORMATTER_H
#define DUEL6_FORMATTER_H

#include <string>
#include <sstream>
#include "Type.h"

namespace Duel6 {
    template<class T>
    class Formatter {
    public:
        std::string format(const T &val) {
            return std::to_string(val);
        }
    };

    template<>
    class Formatter<std::string> {
    public:
        std::string format(const std::string &val) {
            return val;
        }
    };

    template<>
    class Formatter<const char *> {
    public:
        std::string format(const char *const &val) {
            return std::string(val);
        }
    };

    template<Size N>
    class Formatter<const char[N]> {
    private:
        typedef const char Type[N];

    public:
        std::string format(const Type &val) {
            return std::string(val);
        }
    };

    template<Size N>
    class Formatter<char[N]> {
    private:
        typedef char Type[N];

    public:
        std::string format(const Type &val) {
            return std::string(val);
        }
    };

    template<>
    class Formatter<char> {
    public:
        std::string format(const char &val) {
            return std::string(1, val);
        }
    };

    template<>
    class Formatter<bool> {
    public:
        std::string format(const bool &val) {
            return val ? "true" : "false";
        }
    };

    template<class T>
    class Formatter<T *> {
    private:
        typedef T *Type;

    public:
        std::string format(const Type &val) {
            std::stringstream ret;
            ret << "0x" << std::hex << val;
            return ret.str();
        }
    };
}

#endif