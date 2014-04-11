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
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -mfpmath=sse -msse2")
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

    source/anim.cpp
    source/anim.h
    source/anims.h
    source/bonus.cpp
    source/Color.h
    source/duel6.cpp
    source/elevator.cpp
    source/explode.cpp
    source/fire.cpp
    source/glib.cpp
    source/glib.h
    source/info.cpp
    source/kontrola.cpp
    source/levels.h
    source/levels.cpp
    source/loader.cpp
    source/loader.h
    source/menu.cpp
    source/msdir.c
    source/msdir.h
    source/Player.h
    source/Player.cpp
    source/PlayerSkin.h
    source/PlayerSkin.cpp
    source/PlayerSkinColors.h
    source/project.cpp
    source/project.h
    source/render.cpp
    source/setup.cpp
    source/Type.h
    source/util.cpp
    source/water.cpp
    source/weapon.cpp
    
    source/core/co_core.h
    source/core/co_font.cpp
    source/core/co_fps.cpp
    source/core/co_input.cpp
    source/core/co_main.cpp
    source/core/co_vid.cpp
    
    source/mylib/mycam.cpp
    source/mylib/mycam.h
    source/mylib/myerror.cpp
    source/mylib/myfile.cpp
    source/mylib/mykh3.cpp
    source/mylib/mylang.cpp
    source/mylib/mylib.h
    source/mylib/mymath.cpp
    source/mylib/mymath.h
    source/mylib/mytools.cpp
        
    source/console/conarg.cpp
    source/console/concmd.cpp
    source/console/condraw.cpp
    source/console/coninput.cpp
    source/console/console.cpp
    source/console/console.h
    source/console/convar.cpp
    
    source/sound/sound.cpp
    source/sound/sound.h
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
set_target_properties(${D6R_APP_NAME} PROPERTIES VERSION 3.8.12 DEBUG_OUTPUT_NAME ${D6R_APP_DEBUG_NAME})

#########################################################################
# External dependencies
#########################################################################

# OpenGL
if(WIN32)
    target_link_libraries(${D6R_APP_NAME} opengl32.lib)
else(WIN32)
    find_library(LIB_OPEN_GL GL DOC "Path to OpenGL import library")
    target_link_libraries(${D6R_APP_NAME} ${LIB_OPEN_GL})
endif(WIN32)

# SDL
find_path(HEADERS_SDL SDL/SDL.h DOC "Path to SDL headers")
include_directories(${HEADERS_SDL})
if(WIN32)
    find_library(LIB_SDL SDL DOC "Path to SDL import library (release version)")
    find_library(LIB_SDL_MAIN SDLmain DOC "Path to SDLmain import library (release version)")
    find_library(LIB_SDL_MIXER SDL_mixer DOC "Path to SDL_mixer import library (release version)")
    find_library(LIB_SDL_D SDLd DOC "Path to SDL import library (debug version)")
    find_library(LIB_SDL_MAIN_D SDLmaind DOC "Path to SDLmain import library (debug version)")
    find_library(LIB_SDL_MIXER_D SDL_mixerd DOC "Path to SDL_mixer import library (debug version)")
    target_link_libraries(${D6R_APP_NAME} optimized ${LIB_SDL} optimized ${LIB_SDL_MAIN} optimized ${LIB_SDL_MIXER} 
        debug ${LIB_SDL_D} debug ${LIB_SDL_MAIN_D} debug ${LIB_SDL_MIXER_D})
else(WIN32)
    find_library(LIB_SDL SDL DOC "Path to SDL import library")
    find_library(LIB_SDL_MAIN SDLmain DOC "Path to SDLmain import library")
    find_library(LIB_SDL_MIXER SDL_mixer DOC "Path to SDL_mixer import library")
    target_link_libraries(${D6R_APP_NAME} ${LIB_SDL} ${LIB_SDL_MAIN} ${LIB_SDL_MIXER})
endif(WIN32)

########################
#  Install application
########################

# install binary files
install(TARGETS ${D6R_APP_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
