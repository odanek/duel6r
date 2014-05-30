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

#include "File.h"

namespace Duel6
{
	File::File(const std::string& path, const char* mode)
		: handle(nullptr)
	{
		open(path, mode);
	}

	File& File::open(const std::string& path, const char* mode)
	{
		close();
		handle = fopen(path.c_str(), mode);
		if (handle == nullptr)
		{
			// TODO: Exception
		}

		return *this;
	}

	File& File::close()
	{
		if (handle != nullptr)
		{
			fclose(handle);
			handle = nullptr;
		}

		return *this;
	}

	File& File::read(void* ptr, Size size, Size count)
	{
		if (handle == nullptr)
		{
			// TODO: Exception
		}

		if (fread(ptr, size, count, handle) != size * count)
		{
			// TODO: Exception
		}

		return *this;
	}

	File& File::write(const void* ptr, Size size, Size count)
	{
		if (handle == nullptr)
		{
			// TODO: Exception
		}

		if (fwrite(ptr, size, count, handle) != size * count)
		{
			// TODO: Exception
		}

		return *this;
	}
		
	File& File::seek(long offset, Seek seek)
	{
		if (handle == nullptr)
		{
			// TODO: Exception
		}

		int origin;

		switch (seek)
		{
		case Seek::Cur: origin = SEEK_CUR; break;
		case Seek::Set: origin = SEEK_SET; break;
		case Seek::End: origin = SEEK_END; break;
		}

		if (fseek(handle, offset, origin))
		{
			// TODO: Exception
		}

		return *this;
	}

	bool File::isEof() const
	{
		if (handle == nullptr)
		{
			// TODO: Exception
		}

		return (feof(handle) != 0);
	}

	Size File::getSize(const std::string& path)
	{
		FILE* f = fopen(path.c_str(), "rb");
		if (f == nullptr)
		{
			return 0;
		}

		if (fseek(f, 0, SEEK_END))
		{
			// TODO: Exception
		}
		Size length = (Size)ftell(f);
		fclose(f);

		return length;
	}

	bool File::exists(const std::string& path)
	{
		FILE* f = fopen(path.c_str(), "rb");
		if (f != nullptr)
		{
			fclose(f);
			return true;
		}
		return false;
		
	}

	void File::load(const std::string& path, long offset, void* ptr)
	{	
		Size length = getSize(path) - offset;
		File file(path, "rb");
		file.seek(offset, Seek::Set);
		file.read(ptr, 1, length);
		file.close();
	}

	void File::listDirectory(const std::string& path, std::vector<std::string> fileNames)
	{

	}
}