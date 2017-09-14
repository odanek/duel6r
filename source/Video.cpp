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

#include "VideoException.h"
#include "Video.h"

#if defined(D6_RENDERER_GL1)
#include "renderer/GL1Renderer.h"
#elif defined(D6_RENDERER_GLES2)
#include "renderer/GLES2Renderer.h"
#endif

namespace Duel6 {
    Renderer *globRenderer = nullptr; // TODO: Remove

    Video::~Video() {
        if (globRenderer != nullptr) {
            delete globRenderer;
            globRenderer = nullptr;
        }

        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
    }

    void Video::screenUpdate(Console &console, const Font &font) {
        renderConsole(console, font);
        swapBuffers();
    }

    void Video::renderConsole(Console &console, const Font &font) {
        if (console.isActive()) {
            console.render(screen.getClientWidth(), screen.getClientHeight(), font);
        }
    }

    void Video::swapBuffers() {
        SDL_GL_SwapWindow(window);
        calculateFps();
    }

    void Video::calculateFps() {
        static Uint32 curTime = 0, lastTime = 0, frameCounter = 0;
        curTime = SDL_GetTicks();

        if (curTime - lastTime >= 1000) {
            fps = frameCounter * 1000 / float(curTime - lastTime);
            lastTime = curTime;
            frameCounter = 0;
        }
        frameCounter++;
    }

    void Video::initialize(const std::string &name, const std::string &icon, Console &console) {
        console.printLine("\n===Video initialization===");

        // Get current video mode
        SDL_DisplayMode currentVideoMode;
        if (SDL_GetCurrentDisplayMode(0, &currentVideoMode)) {
            D6_THROW(VideoException, std::string("Unable to determine current video mode: ") + SDL_GetError());
        }

        // Set graphics mode
        view = ViewParameters(0.1f, 100.0f, 45.0f);

#ifdef D6_DEBUG
        // Running fullscren makes switching to debugger problematic with SDL (focus is captured)
        screen = ScreenParameters(1280, 900, 32, 0, false);
#else
        screen = ScreenParameters(currentVideoMode.w, currentVideoMode.h, 32, 0, true);
#endif

        window = createWindow(name, icon, screen, console);
        glContext = createContext(screen, console);
        globRenderer = createRenderer();

        setMode(Mode::Orthogonal);

        SDL_ShowCursor(SDL_DISABLE);
    }

    SDL_Window *Video::createWindow(const std::string &name, const std::string &icon, const ScreenParameters &params,
                                    Console &console) {
        Uint32 flags = 0;

        flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_GRABBED;
        if (params.isFullScreen()) {
            flags |= SDL_WINDOW_FULLSCREEN;
        }

        console.printLine(
                Format("...Creating SDL window: {0}x{1}") << params.getClientWidth() << params.getClientHeight());
        SDL_Window *sdlWin = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                              params.getClientWidth(), params.getClientHeight(), flags);
        if (sdlWin == nullptr) {
            D6_THROW(VideoException, std::string("Unable to create application window: ") + SDL_GetError());
        }

        SDL_SetWindowTitle(sdlWin, name.c_str());
        SDL_SetWindowIcon(sdlWin, SDL_LoadBMP(icon.c_str()));

        return sdlWin;
    }

    SDL_GLContext Video::createContext(const ScreenParameters &params, Console &console) {
#if defined(D6_RENDERER_GL1)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
#elif defined(D6_RENDERER_GLES2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif

//      OpenGL 4.3
//		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
//		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
//		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, params.getBitsPerPixel());
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        if (params.getAntiAlias() > 0) {
            console.printLine(Format("...Anti-aliasing: {0}x") << params.getAntiAlias());
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, params.getAntiAlias());
        }

        console.printLine("...Creating OpenGL context");
        console.printLine(Format("...Bits per-pixel: {0}") << params.getBitsPerPixel());
        SDL_GLContext glc = SDL_GL_CreateContext(window);
        if (glc == nullptr) {
            D6_THROW(VideoException, std::string("Unable to OpenGL context: ") + SDL_GetError());
        }

        // Retrieve final video parameters
        int val[7];
        SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &val[0]);
        SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &val[1]);
        SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &val[2]);
        SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &val[3]);
        SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &val[4]);
        SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &val[5]);
        SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &val[6]);

        console.printLine(Format("...RGB ({0}, {1}, {2})") << val[0] << val[1] << val[2]);
        console.printLine(
                Format("...Color ({0}), Z-buffer ({1}), Alpha channel ({2}), Stencil ({3})") << val[3] << val[4]
                                                                                             << val[5] << val[6]);

        return glc;
    }

    Renderer *Video::createRenderer() {
#if defined(D6_RENDERER_GL1)
        Renderer *renderer = new GL1Renderer();
#elif defined(D6_RENDERER_GLES2)
        Renderer* renderer = new GLES2Renderer();
#endif
        renderer->initialize();
        return renderer;
    }

    void Video::setMode(Mode mode) const {
        if (mode == Mode::Perspective) {
            Matrix projection = Matrix::perspective(view.getFieldOfView(), screen.getAspect(), view.getNearClip(),
                                                    view.getFarClip());
            globRenderer->setProjectionMatrix(projection);
            globRenderer->setViewMatrix(Matrix::IDENTITY);
            globRenderer->setModelMatrix(Matrix::IDENTITY);
            globRenderer->enableDepthTest(true);
        } else {
            Matrix projection = Matrix::orthographic(0, screen.getClientWidth(), 0, screen.getClientHeight(), -1, 1);
            globRenderer->setProjectionMatrix(projection);
            globRenderer->setViewMatrix(Matrix::IDENTITY);
            globRenderer->setModelMatrix(Matrix::IDENTITY);
            globRenderer->enableDepthTest(false);
        }
    }
}