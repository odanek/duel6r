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

#ifndef DUEL6_VIDEO_H
#define DUEL6_VIDEO_H

#include <SDL2/SDL.h>
#include "console/console.h"
#include "Type.h"

namespace Duel6
{
	class Video
	{
	public:
		class ScreenParameters
		{
		private:
			Uint32 clientWidth;
			Uint32 clientHeight;
			Uint32 bitsPerPixel;
			Uint32 aaSamples;
			bool fullScreen;

		public:
			ScreenParameters()
			{}

			ScreenParameters(Uint32 width, Uint32 height, Uint32 bpp, Uint32 aa, bool fullScreen)
				: clientWidth(width), clientHeight(height), bitsPerPixel(bpp), aaSamples(aa), fullScreen(fullScreen)
			{}

			Uint32 getClientWidth() const
			{
				return clientWidth;
			}

			Uint32 getClientHeight() const
			{
				return clientHeight;
			}

			bool isFullScreen() const
			{
				return fullScreen;
			}

			Uint32 getBitsPerPixel() const
			{
				return bitsPerPixel;
			}

			Uint32 getAntiAlias() const
			{
				return aaSamples;
			}

			Float32 getAspect() const
			{
				return Float32(clientWidth) / Float32(clientHeight);
			}
		};

		class ViewParameters
		{
		private:
			Float32 nearClip;
			Float32 farClip;
			Float32 fov;

		public:
			ViewParameters()
			{}

			ViewParameters(Float32 nearClip, Float32 farClip, Float32 fov)
				: nearClip(nearClip), farClip(farClip), fov(fov)
			{}

			Float32 getNearClip() const
			{
				return nearClip;
			}

			Float32 getFarClip() const
			{
				return farClip;
			}

			Float32 getFieldOfView() const
			{
				return fov;
			}
		};

		enum class Mode
		{
			Orthogonal,
			Perspective
		};

	private:
		SDL_Window *window;
		SDL_GLContext glContext;
		Float32 fps;		
		ScreenParameters screen;
		ViewParameters view;

	public:
		void initialize(const std::string& name, const std::string& icon, Console& console);
		void swapBuffers(Console& console);
		void setMode(Mode mode) const;
		void closeWindow();

		const ScreenParameters& getScreen() const
		{
			return screen;
		}

		const ViewParameters& getView() const
		{
			return view;
		}

		Float32 getFps() const
		{
			return fps;
		}

	private:
		void calculateFps();
		SDL_Window* createWindow(const std::string& name, const std::string& icon, const ScreenParameters& params, Console& console);
		SDL_GLContext createContext(const ScreenParameters& params, Console& console);
	};
}

#endif