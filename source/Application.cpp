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

#include <SDL2/SDL_image.h>
#ifdef D6_SCRIPTING_LUA
    #include <lua.hpp>
#endif
#include "VideoException.h"

#include "Game.h"
#include "Menu.h"
#include "ConsoleCommands.h"
#include "Application.h"
#include "FontException.h"
#include "net/NetHost.h"
#include "net/NetClient.h"
#include "gamestateintegration/Publisher.h"

namespace Duel6 {
    namespace {
        const Float64 updateTime = 1.0 / D6_UPDATE_FREQUENCY;

        char *getCmdLineOption(char **begin, char **end, const std::string &option) {
            char **itr = std::find(begin, end, option);
            if (itr != end && ++itr != end) {
                return *itr;
            }
            return nullptr;
        }

        bool hasCmdLineOption(char **begin, char **end, const std::string &option) {
            return std::find(begin, end, option) != end;
        }
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
            return MouseMotionEvent(mme.x, screenHeight - mme.y, mme.xrel, -mme.yrel, mme.state);
        }
    }

    Application::Application(Int32 argc, char **argv)
            : console(Console::ExpandFlag), input(console), controlsManager(input), sound(20, console),
              scriptContext(console, sound, gameSettings), scriptManager(scriptContext),
              requestClose(false) {
        parseCommandLineOptions(argc, argv);

#ifndef D6_RENDERER_HEADLESS
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            D6_THROW(VideoException, Format("Unable to set graphics mode: {0}") << SDL_GetError());
        }
#endif
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

        const SDL_version *imgVersion = IMG_Linked_Version();
        console.printLine(
                Format("SDL_image version: {0}.{1}.{2}") << imgVersion->major << imgVersion->minor << imgVersion->patch);

#ifdef D6_SCRIPTING_LUA
        const lua_Number *luaVersion = lua_version(nullptr);
        console.printLine(Format("Lua version: {0}") << *luaVersion);
#endif

        Console::registerBasicCommands(console);

        console.printLine("\n===Video initialization==");
        std::string windowTitle = argv[0];

        ScreenParameters screen(!commandLineOptions.windowed, commandLineOptions.windowWidth, commandLineOptions.windowHeight);
        video = std::make_unique<Video>(windowTitle, APP_FILE_ICON, console, screen);
        textureManager = std::make_unique<TextureManager>(video->getRenderer());

        console.printLine("\n===Font initialization===");
        font = std::make_unique<Font>(video->getRenderer());
        font->load(D6_FILE_TTF_FONT, console);
        clientGameProxy = std::make_unique<net::ClientGameProxy>();
        serverGameProxy = std::make_unique<net::ServerGameProxy>();
        netHost = std::make_unique<net::NetHost>(*clientGameProxy, *serverGameProxy, console);
        netClient = std::make_unique<net::NetClient>(*clientGameProxy, *serverGameProxy, console);
        net = std::make_unique<net::Net>();

      //  netHost->setGameProxyReference(*clientGameProxy, *serverGameProxy);
        service = std::make_unique<AppService>(*font, console, *textureManager, *video, input, controlsManager, sound, scriptManager, *netHost, *netClient);

        gameResources.load(console, sound, *textureManager);

        menu = std::make_unique<Menu>(*service);
        game = std::make_unique<Game>(*service, gameResources, gameSettings);
        clientGameProxy->setGameReference(*game);
        menu->setGameReference(*game);
        menu->setClientGameProxyReference(*clientGameProxy);
        game->setMenuReference(*menu);
        game->setGameProxyReference(*serverGameProxy);
        FireList::initialize();
        console.printLine("\n===Initialize net subsystem===");
        if(!net->initialize()){
            console.printLine("\n===Failed to initialize net subsystem===");
            D6_THROW(Exception, Format("Unable initialize net"));
        }
        for (Weapon weapon : Weapon::values()) {
            gameSettings.enableWeapon(weapon, true);
        }

        scriptManager.registerLoaders();
        menu->initialize();

        // Execute config script and command line arguments
        console.printLine("\n===Config===");
        ConsoleCommands::registerCommands(console, *service, *menu, gameSettings);
        console.exec(std::string("exec ") + D6_FILE_CONFIG);
    }

    Application::~Application() {
        serverGameProxy.reset();
        clientGameProxy.reset();
        netClient.reset();
        netHost.reset();
        net.reset();
        game.reset();
        menu.reset();
        font.reset();
        video.reset();

        TTF_Quit();
        SDL_Quit();
    }

    void Application::parseCommandLineOptions(int argc, char **argv) {
        commandLineOptions.dedicated = hasCmdLineOption(argv, argv+argc, "-dedicated");

        char *window = getCmdLineOption(argv, argv+argc, "-window");
        if (window != nullptr) {
            Int32 width, height;
            sscanf(window, "%dx%d", &width, &height);
            commandLineOptions.windowed = true;
            commandLineOptions.windowWidth = width;
            commandLineOptions.windowHeight = height;
        } else {
            commandLineOptions.windowed = false;
            commandLineOptions.windowWidth = -1;
            commandLineOptions.windowHeight = -1;
        }

        if(hasCmdLineOption(argv, argv + argc, "-publish")){
            commandLineOptions.gameStateIntegration = true;

            char *url = getCmdLineOption(argv, argv + argc, "-url");
            if (url != nullptr) {
                char urlCstr[256];
                sscanf(url, "%s", urlCstr);
                commandLineOptions.gsiURL = std::string(urlCstr);
            }

            char *port = getCmdLineOption(argv, argv + argc, "-port");
            if (port != nullptr) {
                Uint16 portNo;
                sscanf(port, "%hu", &portNo);
                commandLineOptions.gsiPort = portNo;
            }
        }
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
                    if(context.is(*game)){
                        game->announcePeerIsInConsole(true);
                    }
                    SDL_StartTextInput();
                } else if (context.is(*game)) {
                    game->announcePeerIsInConsole(false);
                    SDL_StopTextInput();
                }
            }

            if (console.isActive()) {
                console.keyEvent(event.getCode(), event.getModifiers());
            } else {
                if (event.getCode() == SDLK_RETURN &&
                    ((event.getModifiers() & KMOD_LALT) == KMOD_LALT || (event.getModifiers() & KMOD_RALT) == KMOD_RALT)) {
                    video->toggleFullscreen();
                } else {
                    context.keyEvent(event);
                }
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
    void Application::joyDeviceAddedEvent(Context & context, const JoyDeviceAddedEvent & event) {
        input.joyAttached(event.instance, event.deviceIndex);
        controlsManager.detectJoypads();
        context.joyDeviceAddedEvent(event);
    }
    void Application::joyDeviceRemovedEvent(Context & context, const JoyDeviceRemovedEvent & event) {
        input.joyDetached(event.instanceID);
        controlsManager.detectJoypads();
        context.joyDeviceRemovedEvent(event);
    }
    void Application::windowFocusEvent(Context & context, const WindowFocusEvent & event){
        context.windowFocusEvent(event);
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
                                     createMouseButtonEvent(event.button, video->getScreen().getClientHeight()));
                    break;
                case SDL_MOUSEMOTION:
                    mouseMotionEvent(context,
                                     createMouseMotionEvent(event.motion, video->getScreen().getClientHeight()));
                    break;
                case SDL_MOUSEWHEEL:{
                        Int32 x,y;
                        SDL_GetMouseState(&x, &y);
                        mouseWheelEvent(context, MouseWheelEvent(x, video->getScreen().getClientHeight() - y, event.wheel.x, event.wheel.y));
                    }
                    break;
                case SDL_JOYDEVICEADDED:{
                    auto deviceIndex = event.jdevice.which;
                    auto joy = SDL_JoystickOpen(deviceIndex);
                    if(SDL_JoystickGetAttached(joy)){
                        joyDeviceAddedEvent(context, JoyDeviceAddedEvent(joy, deviceIndex));
                    } else {
                        console.printLine(Format("Joy attached, but has been detached again -> skipping."));
                        break;
                    }

                    break;
                }
                case SDL_JOYDEVICEREMOVED: {
                    auto instanceId = event.jdevice.which;
                    joyDeviceRemovedEvent(context, JoyDeviceRemovedEvent(instanceId));
                    break;
                }
                case SDL_QUIT:
                    console.printLine("Received SDL_QUIT");
                    requestClose = true;
                    break;
                case SDL_WINDOWEVENT: {
                    switch (event.window.event)  {
                      case SDL_WINDOWEVENT_RESIZED:  {
                        int width = event.window.data1;
                        int height = event.window.data2;
                        video->resize(width, height);
                        break;
                      }

                      case SDL_WINDOWEVENT_FOCUS_GAINED: {
                          windowFocusEvent(context, WindowFocusEvent(true));
                          break;
                      }
                      case SDL_WINDOWEVENT_FOCUS_LOST: {
                          windowFocusEvent(context, WindowFocusEvent(false));
                          break;
                      }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    void Application::syncUpdateAndRender(Context &context) {
        static Uint32 curTime = 0;
        static Float64 accumulatedTime = 0.0f;
        Uint32 lastTime = curTime;

        if(accumulatedTime < updateTime){
            context.render();
        }
        video->screenUpdate(console, *font);

        curTime = SDL_GetTicks();
        Float64 elapsedTime = (curTime - lastTime) * 0.001f;
        accumulatedTime += elapsedTime;

        while (accumulatedTime > updateTime) {
            context.update(Float32(updateTime));
            accumulatedTime -= updateTime;
            video->think(updateTime);
        }

        Uint32 residual = (updateTime - accumulatedTime) * 1000;
        if(residual > 3){
            residual = 3;
        }
        netHost->poll(elapsedTime, residual);
        netClient->poll(elapsedTime, residual);
    }

    void Application::run() {
        Publisher publisher(commandLineOptions.gsiURL, commandLineOptions.gsiPort);

        if(commandLineOptions.gameStateIntegration){
            publisher.start();
        }
        Context::push(*menu);
        if (commandLineOptions.dedicated) {
            menu->startDedicatedServer();
        }

        while (Context::exists() && !requestClose) {
            Context &context = Context::getCurrent();
            if(publisher.wantsNewData()){
                publisher.publish(context.getGameState());
            }

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
}
