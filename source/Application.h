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

#ifndef DUEL6_APPLICATION_H
#define DUEL6_APPLICATION_H

#include <memory>
#include "Context.h"
#include "AppService.h"
#include "Menu.h"
#include "Game.h"
#include "Video.h"
#include "script/ScriptManager.h"
#include "net/Net.h"
#include "net/NetHost.h"
#include "net/NetClient.h"
#include "net/ClientGameProxy.h"
#include "net/ServerGameProxy.h"
namespace Duel6 {
    class Application {
    private:
        Console console;
        std::unique_ptr<Video> video;
        std::unique_ptr<Font> font;
        std::unique_ptr<TextureManager> textureManager;
        Input input;
        PlayerControlsManager controlsManager;
        Sound sound;
        GameSettings gameSettings;
        GameResources gameResources;
        Script::ScriptContext scriptContext;
        Script::ScriptManager scriptManager;
        std::unique_ptr<Menu> menu;
        std::unique_ptr<Game> game;
        std::unique_ptr<AppService> service;
        std::unique_ptr<net::NetHost> netHost;
        std::unique_ptr<net::NetClient> netClient;
        std::unique_ptr<net::Net> net;
        std::unique_ptr<net::ClientGameProxy> clientGameProxy;
        std::unique_ptr<net::ServerGameProxy> serverGameProxy;
        bool requestClose;

    public:
        Application(Int32 argc, char **argv);

        ~Application();

        void run();

    private:
        void processEvents(Context &context);

        void textInputEvent(Context &context, const TextInputEvent &event);

        void keyEvent(Context &context, const KeyPressEvent &event);

        void mouseButtonEvent(Context &context, const MouseButtonEvent &event);

        void mouseMotionEvent(Context &context, const MouseMotionEvent &event);

        void mouseWheelEvent(Context &context, const MouseWheelEvent &event);

        void joyDeviceAddedEvent(Context & context, const JoyDeviceAddedEvent & event);
        void joyDeviceRemovedEvent(Context & context, const JoyDeviceRemovedEvent & event);
        void syncUpdateAndRender(Context &context);
    };
}

#endif
