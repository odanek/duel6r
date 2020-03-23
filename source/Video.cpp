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
#include "renderer/gl1/GL1Renderer.h"
#elif defined(D6_RENDERER_GLES2)
#include "renderer/es2/GLES2Renderer.h"
#elif defined(D6_RENDERER_GLES3)
#include "renderer/es3/GLES3Renderer.h"
#elif defined(D6_RENDERER_GL4)

#include "renderer/gl4/GL4Renderer.h"

#endif

namespace Duel6 {
    Video::Video(const std::string &name, const std::string &icon, Console &console) {
        // Get current video mode
        SDL_DisplayMode currentVideoMode;
        if (SDL_GetCurrentDisplayMode(0, &currentVideoMode)) {
            D6_THROW(VideoException, std::string("Unable to determine current video mode: ") + SDL_GetError());
        }

        // Set graphics mode
        view = ViewParameters(1.0f, 40.0f, 45.0f);

//#ifdef D6_DEBUG
        // Running fullscren makes switching to debugger problematic with SDL (focus is captured)
        auto requestedScreenParameters = ScreenParameters(1280, 900, 32, 24, 0, false);
//#else
//        auto requestedScreenParameters = ScreenParameters(currentVideoMode.w, currentVideoMode.h, 32, 24, 0, true);
//#endif

        window = createWindow(name, icon, requestedScreenParameters, console);
        glContext = createContext(requestedScreenParameters, console);
        screen = readScreenParameters(requestedScreenParameters, console);

        GLenum err = glewInit();
        if (GLEW_OK != err) {
            D6_THROW(VideoException, (const char *) glewGetErrorString(err));
        }

        renderer = createRenderer();

        setMode(Mode::Orthogonal);

        SDL_ShowCursor(SDL_DISABLE);
    }

    Video::~Video() {
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
    }

    void Video::screenUpdate(Console &console, const Font &font) {
        renderConsole(console, font);
        swapBuffers();
    }

    void Video::renderConsole(Console &console, const Font &font) {
        if (console.isActive()) {
            console.render(*renderer, screen.getClientWidth(), screen.getClientHeight(), font);
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

    SDL_Window *Video::createWindow(const std::string &name, const std::string &icon, const ScreenParameters &params,
                                    Console &console) {
        Uint32 flags = 0;

        flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN  /*| SDL_WINDOW_INPUT_GRABBED*/;
        if (params.isFullScreen()) {
           // flags |= SDL_WINDOW_FULLSCREEN;
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
        Int32 majorVersion;
        Int32 minorVersion;
        Int32 profile;

#if defined(D6_RENDERER_GL1)
        majorVersion = 2;
        minorVersion = 0;
        profile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
#elif defined(D6_RENDERER_GLES2)
        majorVersion = 2;
        minorVersion = 0;
        profile = SDL_GL_CONTEXT_PROFILE_ES;
#elif defined(D6_RENDERER_GLES3)
        majorVersion = 3;
        minorVersion = 1;
        profile = SDL_GL_CONTEXT_PROFILE_ES;
#elif defined(D6_RENDERER_GL4)
        majorVersion = 4;
        minorVersion = 3;
        profile = SDL_GL_CONTEXT_PROFILE_CORE;
#endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile);

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
            D6_THROW(VideoException, std::string("Unable to create OpenGL context: ") + SDL_GetError());
        }

        return glc;
    }

    std::unique_ptr<Renderer> Video::createRenderer() {
#if defined(D6_RENDERER_GL1)
        return std::make_unique<GL1Renderer>();
#elif defined(D6_RENDERER_GLES2)
        return std::make_unique<GLES2Renderer>();
#elif defined(D6_RENDERER_GLES3)
        return std::make_unique<GLES3Renderer>();
#elif defined(D6_RENDERER_GL4)
        return std::make_unique<GL4Renderer>();
#endif

        D6_THROW(VideoException, "Invalid renderer");
    }

    ScreenParameters Video::readScreenParameters(ScreenParameters &params, Console &console) {
        // Retrieve final video parameters
        int redBits, greenBits, blueBits, bitsPerPixel, depthBits, alphaBits, stencilBits;
        SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &redBits);
        SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &greenBits);
        SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &blueBits);
        SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &bitsPerPixel);
        SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthBits);
        SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alphaBits);
        SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilBits);

        console.printLine(Format("...RGB ({0}, {1}, {2})") << redBits << greenBits << blueBits);
        console.printLine(
                Format("...Color ({0}), Z-buffer ({1}), Alpha channel ({2}), Stencil ({3})") << bitsPerPixel
                                                                                             << depthBits
                                                                                             << alphaBits
                                                                                             << stencilBits);

        return ScreenParameters(params.getClientWidth(), params.getClientHeight(), bitsPerPixel, depthBits,
                                params.getAntiAlias(), params.isFullScreen());
    }

    void Video::setMode(Mode mode) const {
        if (mode == Mode::Perspective) {
            Matrix projection = Matrix::perspective(view.getFieldOfView(), screen.getAspect(), view.getNearClip(),
                                                    view.getFarClip());
            renderer->setProjectionMatrix(projection);
            renderer->setViewMatrix(Matrix::IDENTITY);
            renderer->setModelMatrix(Matrix::IDENTITY);
            renderer->enableDepthTest(true);
        } else {
            Matrix projection = Matrix::orthographic(0, screen.getClientWidth(), 0, screen.getClientHeight(), -1, 1);
            renderer->setProjectionMatrix(projection);
            renderer->setViewMatrix(Matrix::IDENTITY);
            renderer->setModelMatrix(Matrix::IDENTITY);
            renderer->enableDepthTest(false);
        }
    }

    Renderer &Video::getRenderer() const {
        return *renderer;
    }
}
