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

#include <string.h>
#include "msdir.h"
#include "IoException.h"
#include "File.h"
#include "Format.h"

namespace Duel6 {
    File::File(const std::string &path, Mode mode, Access access)
            : handle(nullptr) {
        open(path, mode, access);
    }

    File &File::open(const std::string &path, Mode mode, Access access) {
        close();
        std::string fileMode =
                Format("{0}{1}") << (access == Access::Read ? 'r' : 'w') << (mode == Mode::Text ? 't' : 'b');
        handle = fopen(path.c_str(), fileMode.c_str());
        if (handle == nullptr) {
            D6_THROW(IoException, "Unable to open file: " + path);
        }

        return *this;
    }

    File &File::close() {
        if (handle != nullptr) {
            fclose(handle);
            handle = nullptr;
        }

        return *this;
    }

    File &File::read(void *ptr, Size size, Size count) {
        if (handle == nullptr) {
            D6_THROW(IoException, "Reading from a closed stream");
        }

        if (fread(ptr, size, count, handle) != count) {
            D6_THROW(IoException, "Insufficient data in input stream");
        }

        return *this;
    }

    File &File::write(const void *ptr, Size size, Size count) {
        if (handle == nullptr) {
            D6_THROW(IoException, "Writing to a closed stream");
        }

        if (fwrite(ptr, size, count, handle) != count) {
            D6_THROW(IoException, "Not all data was correctly written");
        }

        return *this;
    }

    File &File::seek(long offset, Seek seek) {
        if (handle == nullptr) {
            D6_THROW(IoException, "Trying to seek in a closed stream");
        }

        int origin;
        switch (seek) {
            case Seek::Set:
                origin = SEEK_SET;
                break;
            case Seek::End:
                origin = SEEK_END;
                break;
            default:
                origin = SEEK_CUR;
                break;
        }

        if (fseek(handle, offset, origin)) {
            D6_THROW(IoException, "Seek operation failed");
        }

        return *this;
    }

    bool File::isEof() const {
        if (handle == nullptr) {
            D6_THROW(IoException, "Querying closed stream for end of file");
        }

        return (feof(handle) != 0);
    }

    Size File::getSize(const std::string &path) {
        FILE *f = fopen(path.c_str(), "rb");
        if (f == nullptr) {
            return 0;
        }

        if (fseek(f, 0, SEEK_END)) {
            D6_THROW(IoException, "Unable to rewind to the end of a stream");
        }
        Size length = (Size) ftell(f);
        fclose(f);

        return length;
    }

    bool File::exists(const std::string &path) {
        FILE *f = fopen(path.c_str(), "rb");
        if (f != nullptr) {
            fclose(f);
            return true;
        }
        return false;

    }

    void File::load(const std::string &path, long offset, void *ptr) {
        Size length = getSize(path) - offset;
        File file(path, File::Mode::Binary, File::Access::Read);
        file.seek(offset, Seek::Set);
        file.read(ptr, 1, length);
        file.close();
    }

    static bool nameEndsWith(const std::string &name, const std::string &suffix) {
        if (name.length() >= suffix.length()) {
            return (name.compare(name.length() - suffix.length(), suffix.length(), suffix) == 0);
        }

        return false;
    }

    void
    File::listDirectory(const std::string &path, const std::string &extension, std::vector<std::string> &fileNames) {
        fileNames.clear();

        DIR *handle = opendir(path.c_str());
        if (handle == nullptr) {
            D6_THROW(IoException, "Invalid directory specified: " + path);
        }

        struct dirent *ff = readdir(handle);

        while (ff != nullptr) {
            bool pseudoDir = (!strcmp(ff->d_name, ".") || !strcmp(ff->d_name, ".."));
            if (!pseudoDir && nameEndsWith(ff->d_name, extension)) {
                fileNames.push_back(ff->d_name);
            }

            ff = readdir(handle);
        }

        closedir(handle);
    }

    Size File::countFiles(const std::string &path, const std::string &extension) {
        DIR *handle = opendir(path.c_str());
        if (handle == nullptr) {
            D6_THROW(IoException, "Invalid directory specified: " + path);
        }

        Size count = 0;
        struct dirent *ff = readdir(handle);

        while (ff != nullptr) {
            bool pseudoDir = (!strcmp(ff->d_name, ".") || !strcmp(ff->d_name, ".."));
            if (!pseudoDir && nameEndsWith(ff->d_name, extension)) {
                ++count;
            }

            ff = readdir(handle);
        }

        closedir(handle);
        return count;
    }
}