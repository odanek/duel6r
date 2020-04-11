cmake_minimum_required(VERSION 3.2)

############################################################################
#
#  Duel 6 Reloaded
#
#  Copyright (C) 2010   Ondrej Danek <ondrej.danek@gmail.com>
#
#############################################################################

# project name
project(Duel6r)

#########################################################################
# Platform detection
#########################################################################

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    EXEC_PROGRAM(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)

    STRING(REGEX REPLACE "^.*(arm).*$" "\\1" ARM_THERE ${CPUINFO})
    STRING(COMPARE EQUAL "arm" "${ARM_THERE}" ARM_TRUE)
    if (ARM_TRUE)
        set(D6R_PLATFORM "arm")
    else (ARM_TRUE)
        set(D6R_PLATFORM "x86")
    endif (ARM_TRUE)
else (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(D6R_PLATFORM "x86")
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

#########################################################################
# Compiler flags
#########################################################################

# visual studio
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
endif (MSVC)

# gnu compiler or clang
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ggdb3 -std=c++17")
    if (D6R_PLATFORM STREQUAL "x86")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -msse2")
    elseif (D6R_PLATFORM STREQUAL "arm")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=hard -mlittle-endian -munaligned-access")
    endif (D6R_PLATFORM STREQUAL "x86")
endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

# MinGW
if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows -static-libgcc -static-libstdc++")
endif (MINGW)

# platform-independent debug macro
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DD6_DEBUG")

# Switches
set(D6R_RENDERERS gl1 gl4 es2 es3)
set(D6R_RENDERER "gl4" CACHE STRING "Renderer")  # Renderer: gl1/gl4/es2
set_property (CACHE D6R_RENDERER PROPERTY STRINGS ${D6R_RENDERERS})
set(D6R_WITH_LUA ON)     # Enable/disable lua scripting

#########################################################################
#
# Duel 6 Reloaded
#
#########################################################################

########################
#  Project source files
########################

# set the list of source files
set(D6R_SOURCES

        source/net/binarystream/BinaryDeserializer.h
        source/net/binarystream/BinarySerializer.h
        source/net/binarystream/BinaryStream.cpp
        source/net/binarystream/BinaryStream.h
        source/net/binarystream/BinaryStreamBase.h
        source/net/binarystream/FileBinaryStream.h
        source/net/binarystream/traits.h

        source/net/event/events.h
        source/net/event/GameState.h
        source/net/event/RequestGameState.h

        source/net/object/objects.h
        source/net/object/Client.h
        source/net/object/Player.h
        source/net/object/Player.cpp
        source/net/object/Shot.h
        source/net/object/Vector.h

        source/net/ClientGameProxy.cpp
        source/net/ClientGameProxy.h
        source/net/Event.h
        source/net/GameProxy.cpp
        source/net/GameProxy.h

        source/net/Message.h
        source/net/Net.cpp
        source/net/Net.h
        source/net/NetClient.cpp
        source/net/NetClient.h
        source/net/NetHost.cpp
        source/net/NetHost.h
        source/net/Object.h
        source/net/Peer.cpp
        source/net/Peer.h
        
        source/net/ServerGameProxy.cpp
        source/net/ServerGameProxy.h
        source/net/Service.h
        source/net/Service.cpp
        source/net/Type.h

        source/AnimationLooping.h
        source/Application.cpp
        source/Application.h
        source/AppService.h
        source/Block.cpp
        source/Block.h
        source/Bonus.cpp
        source/Bonus.h
        source/BonusList.cpp
        source/BonusList.h
        source/Color.cpp
        source/Color.h
        source/ConsoleCommands.cpp
        source/ConsoleCommands.h
        source/Context.cpp
        source/Context.h
        source/DataException.h
        source/Defines.h
        source/Elevator.cpp
        source/Elevator.h
        source/ElevatorList.cpp
        source/ElevatorList.h
        source/EnumClassHash.h
        source/Exception.h
        source/Explosion.cpp
        source/Explosion.h
        source/Face.h
        source/FaceList.cpp
        source/FaceList.h
        source/File.cpp
        source/File.h
        source/Fire.cpp
        source/Fire.h
        source/Font.cpp
        source/Font.h
        source/FontCache.h
        source/FontException.h
        source/Format.cpp
        source/Format.h
        source/FormatException.h
        source/Formatter.h
        source/Game.cpp
        source/Game.h
        source/GameException.h
        source/GameMode.h
        source/GameResources.cpp
        source/GameResources.h
        source/GameSettings.cpp
        source/GameSettings.h
        source/Image.cpp
        source/Image.h
        source/InfoMessage.h
        source/InfoMessageQueue.cpp
        source/InfoMessageQueue.h
        source/IoException.h
        source/Level.cpp
        source/Level.h
        source/LevelList.cpp
        source/LevelList.h
        source/LevelRenderData.cpp
        source/LevelRenderData.h
        source/Main.cpp
        source/Material.h
        source/Menu.cpp
        source/Menu.h
        source/msdir.c
        source/msdir.h
        source/Orientation.h
        source/Person.cpp
        source/Person.h
        source/PersonList.cpp
        source/PersonList.h
        source/PersonProfile.cpp
        source/PersonProfile.h
        source/Player.cpp
        source/Player.h
        source/PlayerAnimations.cpp
        source/PlayerAnimations.h
        source/PlayerEventListener.cpp
        source/PlayerEventListener.h
        source/PlayerIndicators.h
        source/PlayerSkin.cpp
        source/PlayerSkin.h
        source/PlayerSkinColors.cpp
        source/PlayerSkinColors.h
        source/PlayerSounds.cpp
        source/PlayerSounds.h
        source/PlayerView.h
        source/Ranking.h
        source/Rectangle.h
        source/resource.h
        source/Round.cpp
        source/Round.h
        source/ScreenMode.h
        source/ScreenParameters.h
        source/Shot.h
        source/ShotList.cpp
        source/ShotList.h
        source/Sound.cpp
        source/Sound.h
        source/SoundException.h
        source/Sprite.cpp
        source/Sprite.h
        source/SpriteList.cpp
        source/SpriteList.h
        source/SysEvent.h
        source/TextureDictionary.h
        source/TextureManager.cpp
        source/TextureManager.h
        source/Type.h
        source/Vertex.h
        source/Video.cpp
        source/Video.h
        source/VideoException.h
        source/ViewParameters.h
        source/Water.cpp
        source/Water.h
        source/Weapon.cpp
        source/Weapon.h
        source/World.cpp
        source/World.h
        source/WorldRenderer.cpp
        source/WorldRenderer.h

        source/aseprite/animation.h
        source/aseprite/aseprite_to_animation.h
        source/aseprite/aseprite_to_animation.cpp
        source/aseprite/aseprite.cpp
        source/aseprite/aseprite.h
        source/aseprite/tinf/tinf.cpp
        source/aseprite/tinf/tinf.h

        source/bonus/Bullets.cpp
        source/bonus/Bullets.h
        source/bonus/FastMovement.cpp
        source/bonus/FastMovement.h
        source/bonus/FastReload.cpp
        source/bonus/FastReload.h
        source/bonus/FullLife.cpp
        source/bonus/FullLife.h
        source/bonus/InfiniteAmmo.cpp
        source/bonus/InfiniteAmmo.h
        source/bonus/Invisibility.cpp
        source/bonus/Invisibility.h
        source/bonus/Invulnerability.cpp
        source/bonus/Invulnerability.h
        source/bonus/MinusLife.cpp
        source/bonus/MinusLife.h
        source/bonus/PlusLife.cpp
        source/bonus/PlusLife.h
        source/bonus/PowerfulShots.cpp
        source/bonus/PowerfulShots.h
        source/bonus/Snorkel.cpp
        source/bonus/Snorkel.h
        source/bonus/SplitFire.cpp
        source/bonus/SplitFire.h
        source/bonus/VampireShots.cpp
        source/bonus/VampireShots.h

        source/collision/Collision.cpp
        source/collision/Collision.h
        source/collision/WorldCollision.cpp
        source/collision/WorldCollision.h

        source/console/ConsoleArguments.cpp
        source/console/ConsoleCommands.cpp
        source/console/ConsoleRenderer.cpp
        source/console/ConsoleInput.cpp
        source/console/Console.cpp
        source/console/Console.h
        source/console/ConsoleException.h
        source/console/ConsoleVariables.cpp

        source/gamemodes/DeathMatch.cpp
        source/gamemodes/DeathMatch.h
        source/gamemodes/GameModeBase.cpp
        source/gamemodes/GameModeBase.h
        source/gamemodes/Predator.cpp
        source/gamemodes/Predator.h
        source/gamemodes/PredatorPlayerEventListener.cpp
        source/gamemodes/PredatorPlayerEventListener.h
        source/gamemodes/Team.h
        source/gamemodes/TeamDeathMatch.cpp
        source/gamemodes/TeamDeathMatch.h
        source/gamemodes/TeamDeathMatchPlayerEventListener.cpp
        source/gamemodes/TeamDeathMatchPlayerEventListener.h

        source/gui/Button.cpp
        source/gui/Button.h
        source/gui/CheckBox.cpp
        source/gui/CheckBox.h
        source/gui/Control.cpp
        source/gui/Control.h
        source/gui/Desktop.cpp
        source/gui/Desktop.h
        source/gui/Label.cpp
        source/gui/Label.h
        source/gui/ListBox.cpp
        source/gui/ListBox.h
        source/gui/Slider.cpp
        source/gui/Slider.h
        source/gui/Spinner.cpp
        source/gui/Spinner.h
        source/gui/TextBox.cpp
        source/gui/TextBox.h

        source/input/GameController.cpp
        source/input/GameController.h
        source/input/Input.cpp
        source/input/Input.h
        source/input/InputException.h
        source/input/PlayerControls.cpp
        source/input/PlayerControls.h

        source/json/JsonException.h
        source/json/JsonParser.cpp
        source/json/JsonParser.h
        source/json/JsonValue.cpp
        source/json/JsonValue.h
        source/json/JsonWriter.cpp
        source/json/JsonWriter.h

        source/math/Camera.cpp
        source/math/Camera.h
        source/math/Math.cpp
        source/math/Math.h
        source/math/Matrix.cpp
        source/math/Matrix.h
        source/math/Vector.cpp
        source/math/Vector.h

        source/renderer/Renderer.h
        source/renderer/RendererBase.h
        source/renderer/RendererBase.cpp
        source/renderer/RendererBuffer.h
        source/renderer/RendererTarget.h
        source/renderer/RendererTypes.h

        source/script/LevelScript.h
        source/script/PersonScript.h
        source/script/PersonScriptContext.h
        source/script/RoundScriptContext.h
        source/script/Script.h
        source/script/ScriptContext.h
        source/script/ScriptException.h
        source/script/ScriptLoader.h
        source/script/ScriptManager.cpp
        source/script/ScriptManager.h

        source/weapon/LegacyShot.cpp
        source/weapon/LegacyShot.h
        source/weapon/LegacyWeapon.cpp
        source/weapon/LegacyWeapon.h
        source/weapon/ShotBase.cpp
        source/weapon/ShotBase.h
        source/weapon/WeaponBase.cpp
        source/weapon/WeaponBase.h

        source/weapon/impl/Bazooka.cpp
        source/weapon/impl/Bazooka.h
        source/weapon/impl/BazookaShot.cpp
        source/weapon/impl/BazookaShot.h
        source/weapon/impl/Bow.cpp
        source/weapon/impl/Bow.h
        source/weapon/impl/BowShot.cpp
        source/weapon/impl/BowShot.h
        source/weapon/impl/DoubleLaser.cpp
        source/weapon/impl/DoubleLaser.h
        source/weapon/impl/DoubleLaserShot.cpp
        source/weapon/impl/DoubleLaserShot.h
        source/weapon/impl/KissOfDeath.cpp
        source/weapon/impl/KissOfDeath.h
        source/weapon/impl/KissOfDeathShot.cpp
        source/weapon/impl/KissOfDeathShot.h
        source/weapon/impl/Laser.cpp
        source/weapon/impl/Laser.h
        source/weapon/impl/LaserShot.cpp
        source/weapon/impl/LaserShot.h
        source/weapon/impl/Lightning.cpp
        source/weapon/impl/Lightning.h
        source/weapon/impl/LightningShot.cpp
        source/weapon/impl/LightningShot.h
        source/weapon/impl/MachineGun.cpp
        source/weapon/impl/MachineGun.h
        source/weapon/impl/MachineGunShot.cpp
        source/weapon/impl/MachineGunShot.h
        source/weapon/impl/Pistol.cpp
        source/weapon/impl/Pistol.h
        source/weapon/impl/PistolShot.cpp
        source/weapon/impl/PistolShot.h
        source/weapon/impl/Plasma.cpp
        source/weapon/impl/Plasma.h
        source/weapon/impl/PlasmaShot.cpp
        source/weapon/impl/PlasmaShot.h
        source/weapon/impl/ShitThrower.cpp
        source/weapon/impl/ShitThrower.h
        source/weapon/impl/ShitThrowerShot.cpp
        source/weapon/impl/ShitThrowerShot.h
        source/weapon/impl/Shotgun.cpp
        source/weapon/impl/Shotgun.h
        source/weapon/impl/ShotgunShot.cpp
        source/weapon/impl/ShotgunShot.h
        source/weapon/impl/Slime.cpp
        source/weapon/impl/Slime.h
        source/weapon/impl/SlimeShot.cpp
        source/weapon/impl/SlimeShot.h
        source/weapon/impl/Sling.cpp
        source/weapon/impl/Sling.h
        source/weapon/impl/SlingShot.cpp
        source/weapon/impl/SlingShot.h
        source/weapon/impl/Spray.cpp
        source/weapon/impl/Spray.h
        source/weapon/impl/SprayShot.cpp
        source/weapon/impl/SprayShot.h
        source/weapon/impl/StopperGun.cpp
        source/weapon/impl/StopperGun.h
        source/weapon/impl/StopperGunShot.cpp
        source/weapon/impl/StopperGunShot.h
        source/weapon/impl/Triton.cpp
        source/weapon/impl/Triton.h
        source/weapon/impl/TritonShot.cpp
        source/weapon/impl/TritonShot.h
        source/weapon/impl/Uzi.cpp
        source/weapon/impl/Uzi.h
        source/weapon/impl/UziShot.cpp
        source/weapon/impl/UziShot.h
        )

if (D6R_RENDERER STREQUAL "gl1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DD6_RENDERER_GL1")
    set(D6R_SOURCES ${D6R_SOURCES}
            source/renderer/gl1/GL1Buffer.h
            source/renderer/gl1/GL1Buffer.cpp
            source/renderer/gl1/GL1Renderer.h
            source/renderer/gl1/GL1Renderer.cpp
            source/renderer/gl1/GL1RendererTarget.h
            source/renderer/gl1/GL1RendererTarget.cpp
            source/renderer/gl1/GL1Types.h
            )
endif (D6R_RENDERER STREQUAL "gl1")

if (D6R_RENDERER STREQUAL "es2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DD6_RENDERER_GLES2")
    set(D6R_SOURCES ${D6R_SOURCES}
            source/renderer/es2/GLES2Types.h
            source/renderer/es2/GLES2Renderer.h
            source/renderer/es2/GLES2Renderer.cpp
            )
endif (D6R_RENDERER STREQUAL "es2")

if (D6R_RENDERER STREQUAL "es3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DD6_RENDERER_GLES3")
    set(D6R_SOURCES ${D6R_SOURCES}
            source/renderer/es3/GLES3Buffer.h
            source/renderer/es3/GLES3Buffer.cpp
            source/renderer/es3/GLES3Program.h
            source/renderer/es3/GLES3Program.cpp
            source/renderer/es3/GLES3Renderer.h
            source/renderer/es3/GLES3Renderer.cpp
            source/renderer/es3/GLES3RendererTarget.h
            source/renderer/es3/GLES3RendererTarget.cpp
            source/renderer/es3/GLES3Shader.h
            source/renderer/es3/GLES3Shader.cpp
            source/renderer/es3/GLES3Types.h
            )
endif (D6R_RENDERER STREQUAL "es3")


if (D6R_RENDERER STREQUAL "gl4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DD6_RENDERER_GL4")
    set(D6R_SOURCES ${D6R_SOURCES}
            source/renderer/gl4/GL4Buffer.h
            source/renderer/gl4/GL4Buffer.cpp
            source/renderer/gl4/GL4Renderer.h
            source/renderer/gl4/GL4Renderer.cpp
            source/renderer/gl4/GL4RendererTarget.h
            source/renderer/gl4/GL4RendererTarget.cpp
            source/renderer/gl4/GL4Shader.h
            source/renderer/gl4/GL4Shader.cpp
            source/renderer/gl4/GL4Program.h
            source/renderer/gl4/GL4Program.cpp
            source/renderer/gl4/GL4Types.h
        )
endif (D6R_RENDERER STREQUAL "gl4")

if (D6R_WITH_LUA)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DD6_SCRIPTING_LUA")
    set(D6R_SOURCES ${D6R_SOURCES}
            source/script/lua/Lua.cpp
            source/script/lua/Lua.h
            source/script/lua/LuaPersonScript.h
            source/script/lua/LuaPersonScript.cpp
            source/script/lua/LuaLevelScript.h
            source/script/lua/LuaLevelScript.cpp
            source/script/lua/LuaScriptLoader.h
            source/script/lua/LuaScriptLoader.cpp
            )
endif (D6R_WITH_LUA)

if (WIN32)
    set(D6R_SOURCES ${D6R_SOURCES} source/duel6r.rc)
endif (WIN32)

########################
#  Add application
########################

set(D6R_APP_NAME "duel6r" CACHE STRING "Filename of the application.")
set(D6R_APP_DEBUG_NAME "duel6rd" CACHE STRING "Filename of the debug version of the application.")
add_executable(${D6R_APP_NAME} ${D6R_SOURCES})
set_target_properties(${D6R_APP_NAME} PROPERTIES VERSION 6.0.0 DEBUG_OUTPUT_NAME ${D6R_APP_DEBUG_NAME})

#########################################################################
# External dependencies
#########################################################################

# Platform
if (MINGW)
    target_link_libraries(${D6R_APP_NAME} mingw32)
endif (MINGW)

# OpenGL
if (WIN32)
    target_link_libraries(${D6R_APP_NAME} opengl32.lib)
elseif (APPLE)
    find_library(LIB_OPEN_GL OpenGL DOC "Path to OpenGL framework")
    target_link_libraries(${D6R_APP_NAME} ${LIB_OPEN_GL})
else (WIN32) # Linux GCC
    find_library(LIB_OPEN_GL GL DOC "Path to OpenGL import library")
    target_link_libraries(${D6R_APP_NAME} ${LIB_OPEN_GL})
endif (WIN32)

# SDL
find_path(HEADERS_SDL2 SDL2/SDL.h DOC "Path to SDL2 headers")
include_directories(${HEADERS_SDL2})

find_library(LIB_SDL2 SDL2 DOC "Path to SDL2 import library")
find_library(LIB_SDL2_MAIN SDL2main DOC "Path to SDL2main import library")
find_library(LIB_SDL2_MIXER SDL2_mixer DOC "Path to SDL2_mixer import library")
find_library(LIB_SDL2_TTF SDL2_ttf DOC "Path to SDL2_ttf import library")
find_library(LIB_SDL2_IMAGE SDL2_image DOC "Path to SDL2_image import library")
target_link_libraries(${D6R_APP_NAME} ${LIB_SDL2_MAIN} ${LIB_SDL2} ${LIB_SDL2_MIXER} ${LIB_SDL2_TTF} ${LIB_SDL2_IMAGE})


find_path(HEADERS_ENET enet/enet.h DOC "Path to ENet headers")
include_directories(${HEADERS_ENET})
find_library(LIB_ENET NAMES enet enet_static libenet libenet_static DOC "Path to ENet library")
target_link_libraries(${D6R_APP_NAME} ${LIB_ENET})

# GLEW
if (WIN32)
    find_library(LIB_GLEW glew32 DOC "Path to GLEW library")
else (WIN32)
    find_library(LIB_GLEW GLEW DOC "Path to GLEW library")
endif (WIN32)
target_link_libraries(${D6R_APP_NAME} ${LIB_GLEW})
find_path(HEADERS_GLEW GL/glew.h DOC "Path to GLEW headers")
include_directories(${HEADERS_GLEW})

# LUA
if (D6R_WITH_LUA)
    if (WIN32)
        find_library(LIB_LUA lua DOC "Path to LUA library")
        target_link_libraries(${D6R_APP_NAME} ${LIB_LUA})
        find_path(HEADERS_LUA lua.hpp DOC "Path to LUA headers")
        include_directories(${HEADERS_LUA})
    else (WIN32)
        find_library(LIB_LUA lua5.3 DOC "Path to LUA library")
        target_link_libraries(${D6R_APP_NAME} ${LIB_LUA})
        find_path(HEADERS_LUA lua5.3/lua.hpp DOC "Path to LUA headers")
        include_directories(${HEADERS_LUA}/lua5.3)
    endif (WIN32)
endif (D6R_WITH_LUA)