cmake_minimum_required(VERSION 2.6)

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
# compilation details
#########################################################################

# turn off some warnings under msc
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")    
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
endif(WIN32)

# show all warnings under gcc
if(UNIX)
	# gnu compiler
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-result -mfpmath=sse -msse2 -std=c++11")
	endif(CMAKE_COMPILER_IS_GNUCXX)
endif(UNIX)       

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
    source/Block.h
    source/Bonus.h
    source/Bonus.cpp
    source/BonusList.h
    source/BonusList.cpp
    source/Color.h
    source/ConsoleCommands.h
    source/ConsoleCommands.cpp
    source/Context.h
    source/Context.cpp
    source/Elevator.h
    source/Elevator.cpp
    source/ElevatorList.h
    source/ElevatorList.cpp
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
    source/Format.h
    source/Format.cpp
    source/Formatter.h
    source/Game.cpp
    source/Game.h
    source/Image.h
    source/Gui.h
    source/Gui.cpp
    source/Globals.h
    source/InfoMessage.h
    source/InfoMessageQueue.h
    source/InfoMessageQueue.cpp
    source/Lang.h
    source/Lang.cpp
    source/LevelList.h
    source/LevelList.cpp
    source/Main.h
    source/Main.cpp
    source/Math.h
    source/Menu.h
    source/Menu.cpp
    source/msdir.c
    source/msdir.h
    source/Orientation.h
    source/Person.h
    source/Person.cpp
    source/PersonList.h
    source/PersonList.cpp
    source/Player.h
    source/Player.cpp
    source/PlayerControls.h
    source/PlayerControls.cpp
    source/PlayerSkin.h
    source/PlayerSkin.cpp
    source/PlayerSkinColors.h
    source/Render.h
    source/Render.cpp
    source/ScreenMode.h
    source/Shot.h
    source/Shot.cpp
    source/Sound.cpp
    source/Sound.h
    source/Sprite.h
    source/Sprite.cpp
    source/SpriteList.h
    source/SpriteList.cpp
    source/TextureManager.h
    source/TextureManager.cpp
    source/Type.h
    source/Util.h
    source/Util.cpp
    source/Vertex.h
    source/Video.h
    source/Video.cpp    
    source/WaterList.h
    source/WaterList.cpp
    source/WaterType.h
    source/Weapon.h
    source/Weapon.cpp
    source/World.h
    source/World.cpp
    
    source/DataException.h
    source/Exception.h
    source/IoException.h
    source/SoundException.h
    source/VideoException.h
    
    source/mylib/mycam.cpp
    source/mylib/mycam.h
    source/mylib/mymath.cpp
    source/mylib/mymath.h
        
    source/console/conarg.cpp
    source/console/concmd.cpp
    source/console/condraw.cpp
    source/console/coninput.cpp
    source/console/console.cpp
    source/console/console.h
    source/console/convar.cpp    
    source/console/ConsoleException.h
)

if(WIN32)
    set(D6R_SOURCES ${D6R_SOURCES} source/duel6r.rc)
endif(WIN32)

########################
#  Add application
########################

set(D6R_APP_NAME "duel6r" CACHE STRING "Filename of the application.")
set(D6R_APP_DEBUG_NAME "duel6rd" CACHE STRING "Filename of the debug version of the application.")
add_executable(${D6R_APP_NAME} ${D6R_SOURCES})
set_target_properties(${D6R_APP_NAME} PROPERTIES VERSION 3.11.0 DEBUG_OUTPUT_NAME ${D6R_APP_DEBUG_NAME})

#########################################################################
# External dependencies
#########################################################################

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
find_path(HEADERS_SDL2 SDL2/SDL.h DOC "Path to SDL2 headers")
include_directories(${HEADERS_SDL2})
if(WIN32)
    find_library(LIB_SDL2 SDL2 DOC "Path to SDL2 import library (release version)")
    find_library(LIB_SDL2_MAIN SDL2main DOC "Path to SDL2main import library (release version)")
    find_library(LIB_SDL2_MIXER SDL2_mixer DOC "Path to SDL2_mixer import library (release version)")
    find_library(LIB_SDL2_D SDL2d DOC "Path to SDL2 import library (debug version)")
    find_library(LIB_SDL2_MAIN_D SDL2maind DOC "Path to SDL2main import library (debug version)")
    find_library(LIB_SDL2_MIXER_D SDL2_mixerd DOC "Path to SDL2_mixer import library (debug version)")
    target_link_libraries(${D6R_APP_NAME} optimized ${LIB_SDL2} optimized ${LIB_SDL2_MAIN} optimized ${LIB_SDL2_MIXER} 
        debug ${LIB_SDL2_D} debug ${LIB_SDL2_MAIN_D} debug ${LIB_SDL2_MIXER_D})
elseif(APPLE)
    find_library(LIB_SDL2 SDL2 DOC "Path to SDL2 framework")
    find_library(LIB_SDL2_MIXER SDL2_mixer DOC "Path to SDL2_mixer import library")
    target_link_libraries(${D6R_APP_NAME} ${LIB_SDL2} ${LIB_SDL2_MIXER})    
else(WIN32) # Linux GCC
    find_library(LIB_SDL2 SDL2 DOC "Path to SDL2 import library")
    find_library(LIB_SDL2_MAIN SDL2main DOC "Path to SDL2main import library")
    find_library(LIB_SDL2_MIXER SDL2_mixer DOC "Path to SDL2_mixer import library")
    target_link_libraries(${D6R_APP_NAME} ${LIB_SDL2} ${LIB_SDL2_MAIN} ${LIB_SDL2_MIXER})
endif(WIN32)

########################
#  Install application
########################

# install binary files
install(TARGETS ${D6R_APP_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
