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
#include "InfoMessageQueue.h"
#include "Game.h"
#include "Menu.h"
#include "ConsoleCommands.h"
#include "Application.h"
#include "FontException.h"

namespace Duel6 {
    namespace {
        static const Float64 updateTime = 1.0 / D6_UPDATE_FREQUENCY;
    }

    namespace {
        KeyPressEvent createKeyPressEvent(const SDL_KeyboardEvent &ke) {
            SDL_Keysym key = ke.keysym;
            SysEvent::ButtonState buttonState =
                    ke.type == SDL_KEYDOWN ? SysEvent::ButtonState::PRESSED : SysEvent::ButtonState::RELEASED;
            return KeyPressEvent(key.sym, buttonState, key.mod);
        }

        SysEvent::MouseButton getMouseButton(Int32 sdlButton) {
            if (sdlButton == SDL_BUTTON_LEFT) {
                return SysEvent::MouseButton::LEFT;
            } else if (sdlButton == SDL_BUTTON_RIGHT) {
                return SysEvent::MouseButton::RIGHT;
            } else if (sdlButton == SDL_BUTTON_MIDDLE) {
                return SysEvent::MouseButton::MIDDLE;
            }
            return SysEvent::MouseButton::NONE;
        }

        MouseButtonEvent createMouseButtonEvent(const SDL_MouseButtonEvent &mbe, Int32 screenHeight) {
            SysEvent::MouseButton button = getMouseButton(mbe.button);
            SysEvent::ButtonState state = (mbe.state == SDL_PRESSED) ? SysEvent::ButtonState::PRESSED
                                                                     : SysEvent::ButtonState::RELEASED;
            return MouseButtonEvent(mbe.x, screenHeight - mbe.y, button, state, mbe.clicks == 2);
        }

        MouseMotionEvent createMouseMotionEvent(const SDL_MouseMotionEvent &mme, Int32 screenHeight) {
            return MouseMotionEvent(mme.x, screenHeight - mme.y, mme.xrel, mme.yrel, mme.state);
        }
    }

    Application::Application()
            : console(Console::ExpandFlag), textureManager(D6_TEXTURE_EXTENSION), sound(20, console),
              service(font, console, textureManager, video, input, sound),
              menu(service), requestClose(false) {}

    Application::~Application() {
        tearDown();
    }

    void Application::textInputEvent(Context &context, const TextInputEvent &event) {
        if (console.isActive()) {
            console.textInputEvent(event.getText());
        } else {
            context.textInputEvent(event);
        }
    }

    void Application::keyEvent(Context &context, const KeyPressEvent &event) {
        input.setPressed(event.getCode(), event.isPressed());

        if (event.isPressed()) {
            if (event.getCode() == SDLK_BACKQUOTE) {
                console.toggle();
                if (console.isActive()) {
                    SDL_StartTextInput();
                } else if (context.is(*game)) {
                    SDL_StopTextInput();
                }
            }

            if (console.isActive()) {
                console.keyEvent(event.getCode(), event.getModifiers());
            } else {
                context.keyEvent(event);
            }
        }
    }

    void Application::mouseButtonEvent(Context &context, const MouseButtonEvent &event) {
        context.mouseButtonEvent(event);
    }

    void Application::mouseMotionEvent(Context &context, const MouseMotionEvent &event) {
        context.mouseMotionEvent(event);
    }

    void Application::mouseWheelEvent(Context &context, const MouseWheelEvent &event) {
        context.mouseWheelEvent(event);
    }

    void Application::processEvents(Context &context) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    keyEvent(context, createKeyPressEvent(event.key));
                    break;
                case SDL_TEXTINPUT:
                    textInputEvent(context, TextInputEvent(event.text.text));
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    mouseButtonEvent(context,
                                     createMouseButtonEvent(event.button, video.getScreen().getClientHeight()));
                    break;
                case SDL_MOUSEMOTION:
                    mouseMotionEvent(context,
                                     createMouseMotionEvent(event.motion, video.getScreen().getClientHeight()));
                    break;
                case SDL_MOUSEWHEEL:
                    mouseWheelEvent(context, MouseWheelEvent(0, 0, event.wheel.x, event.wheel.y));
                    break;
                case SDL_QUIT:
                    requestClose = true;
                    break;
                default:
                    break;
            }
        }
    }

    void Application::syncUpdateAndRender(Context &context) {
        static Uint32 curTime = 0;
        static Float64 accumulatedTime = 0.0f;
        Uint32 lastTime = curTime;

        context.render();
        video.screenUpdate(console, font);

        curTime = SDL_GetTicks();
        Float64 elapsedTime = (curTime - lastTime) * 0.001f;
        accumulatedTime += elapsedTime;

        while (accumulatedTime > updateTime) {
            context.update(Float32(updateTime));
            accumulatedTime -= updateTime;
        }
    }

    void Application::setup(Int32 argc, char **argv) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            D6_THROW(VideoException, Format("Unable to set graphics mode: {0}") << SDL_GetError());
        }
        if (TTF_Init() != 0) {
            D6_THROW(FontException, Format("Unable to initialize font subsystem: {0}") << TTF_GetError());
        }

        // Print application info
        SDL_version sdlVersion;
        console.printLine("\n===Application information===");
        console.printLine(Format("{0} version: {1}") << APP_NAME << APP_VERSION);
        SDL_GetVersion(&sdlVersion);
        console.printLine(
                Format("SDL version: {0}.{1}.{2}") << sdlVersion.major << sdlVersion.minor << sdlVersion.patch);
        const SDL_version *mixVersion = Mix_Linked_Version();
        console.printLine(Format("SDL_mixer version: {0}.{1}.{2}") << mixVersion->major << mixVersion->minor
                                                                   << mixVersion->patch);
        const SDL_version *ttfVersion = TTF_Linked_Version();
        console.printLine(
                Format("SDL_ttf version: {0}.{1}.{2}") << ttfVersion->major << ttfVersion->minor << ttfVersion->patch);

        Console::registerBasicCommands(console);
        ConsoleCommands::registerCommands(console, service, menu, gameSettings);

        console.printLine("\n===Font initialization===");
        font.load(D6_FILE_TTF_FONT, console);

        video.initialize(APP_NAME, APP_FILE_ICON, console);
        menu.initialize();

        gameResources = std::make_unique<GameResources>(service);
        game = std::make_unique<Game>(service, *gameResources, gameSettings);
        menu.setGameReference(game.get());
        game->setMenuReference(&menu);

        for (Weapon weapon : Weapon::values()) {
            gameSettings.enableWeapon(weapon, true);
        }

        // Execute config script and command line arguments
        console.printLine("\n===Config===");
        console.exec(std::string("exec ") + D6_FILE_CONFIG);

        for (int i = 1; i < argc; i++) {
            console.exec(argv[i]);
        }
    }

    void Application::run() {
        Context::push(menu);

        while (Context::exists() && !requestClose) {
            Context &context = Context::getCurrent();
            processEvents(context);
            syncUpdateAndRender(context);

            if (context.isClosed()) {
                Context::pop();
            }
        }

        while (Context::exists()) // Pop all contexts (if any) to execute all beforeClose logic
        {
            Context::pop();
        }
    }

    void Application::tearDown() {
        textureManager.disposeAll();
        font.dispose();
        TTF_Quit();
        SDL_Quit();
    }
}