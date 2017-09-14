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
# Compiler flags
#########################################################################

# visual studio
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")    
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

# gnu compiler or clang
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -mfpmath=sse -msse2 -std=c++14")
endif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

# MinGW
if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows -static-libgcc -static-libstdc++")
endif(MINGW)

# platform-independent debug macro
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DD6_DEBUG")

# Renderer switch
set(D6R_RENDERER "gl1")

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

    source/AnimationLooping.h
    source/Application.h
    source/Application.cpp
	source/AppService.h
    source/Block.h
    source/Block.cpp
    source/Bonus.h
    source/Bonus.cpp
    source/BonusList.h
    source/BonusList.cpp
	source/Camera.h
	source/Camera.cpp
    source/Color.h
    source/Color.cpp
    source/ConsoleCommands.h
    source/ConsoleCommands.cpp
    source/Context.h
    source/Context.cpp
    source/DataException.h
	source/Defines.h
    source/Elevator.h
    source/Elevator.cpp
    source/ElevatorList.h
    source/ElevatorList.cpp
    source/Exception.h
    source/EnumClassHash.h
    source/Explosion.h
    source/Explosion.cpp
    source/Face.h
    source/FaceList.h
    source/FaceList.cpp
    source/File.h
    source/File.cpp
    source/Fire.h
    source/Fire.cpp
    source/Font.h
    source/Font.cpp
    source/FontException.h
    source/Format.h
    source/Format.cpp
    source/FormatException.h
    source/Formatter.h
    source/Game.cpp
    source/Game.h
    source/GameMode.h
	source/GameException.h
	source/GameResources.h
	source/GameResources.cpp
	source/GameSettings.h
	source/GameSettings.cpp
    source/Image.h
    source/InfoMessage.h
    source/InfoMessageQueue.h
    source/InfoMessageQueue.cpp
    source/Input.h
    source/Input.cpp
    source/IoException.h
    source/Level.h
    source/Level.cpp
    source/LevelList.h
    source/LevelList.cpp
    source/LevelRenderData.h
    source/LevelRenderData.cpp
    source/Main.cpp
	source/Material.h
    source/Math.h
    source/Math.cpp
	source/Matrix.h
	source/Matrix.cpp
    source/Menu.h
    source/Menu.cpp
    source/msdir.c
    source/msdir.h
    source/Orientation.h
    source/Person.h
    source/Person.cpp
    source/PersonList.h
    source/PersonList.cpp
	source/PersonProfile.h
	source/PersonProfile.cpp
    source/Player.h
    source/Player.cpp
    source/PlayerControls.h
    source/PlayerControls.cpp
    source/PlayerSkin.h
    source/PlayerSkin.cpp
    source/PlayerSkinColors.h
	source/PlayerSkinColors.cpp
	source/PlayerSounds.h
	source/PlayerSounds.cpp
    source/Ranking.h
	source/Rectangle.h
	source/WorldRenderer.h
	source/WorldRenderer.cpp
    source/Round.h
    source/Round.cpp
    source/ScreenMode.h
	source/ScreenParameters.h
    source/Shot.h
    source/ShotList.h
    source/ShotList.cpp
    source/Sound.cpp
    source/Sound.h
    source/SoundException.h
    source/Sprite.h
    source/Sprite.cpp
    source/SpriteList.h
    source/SpriteList.cpp
    source/SysEvent.h
    source/Texture.h
    source/TextureManager.h
    source/TextureManager.cpp
    source/Type.h
    source/Util.h
    source/Util.cpp
    source/Vector.h
    source/Vector.cpp
    source/Vertex.h
    source/Video.h
    source/Video.cpp
    source/VideoException.h
	source/ViewParameters.h
	source/Water.cpp
	source/Water.h
    source/WaterList.h
    source/WaterList.cpp
    source/Weapon.h
    source/Weapon.cpp
    source/World.h
    source/World.cpp

	source/console/conarg.cpp
    source/console/concmd.cpp
    source/console/condraw.cpp
    source/console/coninput.cpp
    source/console/console.cpp
    source/console/console.h
    source/console/convar.cpp    
    source/console/ConsoleException.h

	source/json/JsonException.h
	source/json/JsonParser.cpp
	source/json/JsonParser.h
	source/json/JsonValue.cpp
	source/json/JsonValue.h
	source/json/JsonWriter.cpp
	source/json/JsonWriter.h

	source/gui/Button.h
	source/gui/Button.cpp
	source/gui/Control.h
	source/gui/Control.cpp
	source/gui/Desktop.h
	source/gui/Desktop.cpp
	source/gui/Label.h
	source/gui/Label.cpp
	source/gui/ListBox.h
	source/gui/ListBox.cpp
	source/gui/Slider.h
	source/gui/Slider.cpp
	source/gui/Spinner.h
	source/gui/Spinner.cpp
	source/gui/TextBox.h
	source/gui/TextBox.cpp

    source/bonus/Bullets.h
    source/bonus/Bullets.cpp
    source/bonus/FastMovement.h
    source/bonus/FastMovement.cpp
    source/bonus/FastReload.h
    source/bonus/FastReload.cpp
    source/bonus/FullLife.h
    source/bonus/FullLife.cpp
    source/bonus/Invisibility.h
    source/bonus/Invisibility.cpp
    source/bonus/Invulnerability.h
    source/bonus/Invulnerability.cpp
    source/bonus/MinusLife.h
    source/bonus/MinusLife.cpp
    source/bonus/PlusLife.h
    source/bonus/PlusLife.cpp
    source/bonus/PowerfulShots.h
    source/bonus/PowerfulShots.cpp
    source/bonus/SplitFire.h
    source/bonus/SplitFire.cpp
    source/bonus/VampireShots.h
    source/bonus/VampireShots.cpp

    source/weapon/WeaponBase.h
    source/weapon/WeaponBase.cpp
    source/weapon/LegacyWeapon.h
    source/weapon/LegacyWeapon.cpp
    source/weapon/ShotBase.h
    source/weapon/ShotBase.cpp
    source/weapon/LegacyShot.h
    source/weapon/LegacyShot.cpp
    source/weapon/impl/ShitThrower.h
    source/weapon/impl/ShitThrower.cpp
    source/weapon/impl/ShitThrowerShot.h
    source/weapon/impl/ShitThrowerShot.cpp
    source/weapon/impl/Pistol.cpp
    source/weapon/impl/Pistol.h
    source/weapon/impl/Shotgun.cpp
    source/weapon/impl/Shotgun.h
    source/weapon/impl/Uzi.cpp
    source/weapon/impl/Uzi.h
    source/weapon/impl/Bazooka.cpp
    source/weapon/impl/Bazooka.h
    source/weapon/impl/Plasma.cpp
    source/weapon/impl/Plasma.h
    source/weapon/impl/Laser.cpp
    source/weapon/impl/Laser.h
    source/weapon/impl/Lightning.cpp
    source/weapon/impl/Lightning.h
    source/weapon/impl/MachineGun.cpp
    source/weapon/impl/MachineGun.h
    source/weapon/impl/Slime.cpp
    source/weapon/impl/Slime.h
    source/weapon/impl/Triton.cpp
    source/weapon/impl/Triton.h
    source/weapon/impl/Bow.cpp
    source/weapon/impl/Bow.h
    source/weapon/impl/DoubleLaser.cpp
    source/weapon/impl/DoubleLaser.h
    source/weapon/impl/KissOfDeath.cpp
    source/weapon/impl/KissOfDeath.h
    source/weapon/impl/Sling.cpp
    source/weapon/impl/Sling.h
    source/weapon/impl/Spray.cpp
    source/weapon/impl/Spray.h
    source/weapon/impl/StopperGun.cpp
    source/weapon/impl/StopperGun.h

    source/collision/Collision.h
    source/collision/Collision.cpp
	source/collision/WorldCollision.cpp
	source/collision/WorldCollision.h

	source/gamemodes/GameModeBase.h
	source/gamemodes/GameModeBase.cpp
	source/gamemodes/TeamDeathMatchPlayerEventListener.h
	source/gamemodes/TeamDeathMatchPlayerEventListener.cpp
	source/gamemodes/PredatorPlayerEventListener.h
	source/gamemodes/PredatorPlayerEventListener.cpp
    source/gamemodes/DeathMatch.h
    source/gamemodes/DeathMatch.cpp
    source/gamemodes/Team.h
    source/gamemodes/TeamDeathMatch.h
    source/gamemodes/TeamDeathMatch.cpp
    source/gamemodes/Predator.cpp
    source/gamemodes/Predator.h

	source/renderer/Renderer.h

    source/PlayerEventListener.h 
    source/PlayerEventListener.cpp
    )

if(D6R_RENDERER STREQUAL "gl1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DD6_RENDERER_GL1")
	set(D6R_SOURCES ${D6R_SOURCES} source/renderer/GL1Renderer.h source/renderer/GL1Renderer.cpp)
endif(D6R_RENDERER STREQUAL "gl1")

if(D6R_RENDERER STREQUAL "es2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DD6_RENDERER_GLES2")
	set(D6R_SOURCES ${D6R_SOURCES} source/renderer/GLES2Renderer.h source/renderer/GLES2Renderer.cpp)

	if(WIN32)
		set(D6R_WITH_GLEW ON)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DD6_GLEW")
	endif(WIN32)
endif(D6R_RENDERER STREQUAL "es2")

if(WIN32)
    set(D6R_SOURCES ${D6R_SOURCES} source/duel6r.rc)
endif(WIN32)

########################
#  Add application
########################

set(D6R_APP_NAME "duel6r" CACHE STRING "Filename of the application.")
set(D6R_APP_DEBUG_NAME "duel6rd" CACHE STRING "Filename of the debug version of the application.")
add_executable(${D6R_APP_NAME} ${D6R_SOURCES})
set_target_properties(${D6R_APP_NAME} PROPERTIES VERSION 4.0.1 DEBUG_OUTPUT_NAME ${D6R_APP_DEBUG_NAME})

add_custom_command(TARGET ${D6R_APP_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources/ $<TARGET_FILE_DIR:${D6R_APP_NAME}>)

#########################################################################
# External dependencies
#########################################################################

# Platform
if (MINGW)
    target_link_libraries(${D6R_APP_NAME} mingw32)
endif(MINGW)

# OpenGL
if(WIN32)
    target_link_libraries(${D6R_APP_NAME} opengl32.lib)
elseif(APPLE)
    find_library(LIB_OPEN_GL OpenGL DOC "Path to OpenGL framework")
    target_link_libraries(${D6R_APP_NAME} ${LIB_OPEN_GL})
else(WIN32) # Linux GCC
    find_library(LIB_OPEN_GL GL DOC "Path to OpenGL import library")
    target_link_libraries(${D6R_APP_NAME} ${LIB_OPEN_GL})
endif(WIN32)

# SDL
if(APPLE)
    find_library(LIB_SDL2 SDL2 DOC "Path to SDL2 framework")
    find_library(LIB_SDL2_MIXER SDL2_mixer DOC "Path to SDL2_mixer framework")
    find_library(LIB_SDL2_TTF SDL2_ttf DOC "Path to SDL2_ttf framework")
    target_link_libraries(${D6R_APP_NAME} ${LIB_SDL2} ${LIB_SDL2_MIXER} ${LIB_SDL2_TTF})
else(APPLE) # MSVC, GCC or MINGW
    find_path(HEADERS_SDL2 SDL2/SDL.h DOC "Path to SDL2 headers")
    include_directories(${HEADERS_SDL2})

    find_library(LIB_SDL2 SDL2 DOC "Path to SDL2 import library")
    find_library(LIB_SDL2_MAIN SDL2main DOC "Path to SDL2main import library")
    find_library(LIB_SDL2_MIXER SDL2_mixer DOC "Path to SDL2_mixer import library")
    find_library(LIB_SDL2_TTF SDL2_ttf DOC "Path to SDL2_ttf import library")
    target_link_libraries(${D6R_APP_NAME} ${LIB_SDL2_MAIN} ${LIB_SDL2} ${LIB_SDL2_MIXER} ${LIB_SDL2_TTF})
endif(APPLE)

# GLEW
if(D6R_WITH_GLEW)
	find_library(LIB_GLEW glew32 DOC "Path to GLEW library")
	target_link_libraries(${D6R_APP_NAME} ${LIB_GLEW})
	find_path(HEADERS_GLEW GL/glew.h DOC "Path to GLEW headers")
	include_directories(${HEADERS_GLEW})
endif(D6R_WITH_GLEW)

########################
#  Install application
########################

# install binary files
install(TARGETS ${D6R_APP_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
#install(DIRECTORY resources/data DESTINATION data)
#install(DIRECTORY resources/levels DESTINATION levels)
#install(DIRECTORY resources/sound DESTINATION sound)
#install(DIRECTORY resources/textures DESTINATION textures)
