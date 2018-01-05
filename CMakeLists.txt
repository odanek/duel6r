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

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	EXEC_PROGRAM(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)

	STRING(REGEX REPLACE "^.*(arm).*$" "\\1" ARM_THERE ${CPUINFO})
	STRING(COMPARE EQUAL "arm" "${ARM_THERE}" ARM_TRUE)
	if(ARM_TRUE)
		set(D6R_PLATFORM "arm")
	else(ARM_TRUE)
		set(D6R_PLATFORM "x86")
	endif(ARM_TRUE)
else(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(D6R_PLATFORM "x86")
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
	if(D6R_PLATFORM STREQUAL "x86")

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -msse2")
	elseif(D6R_PLATFORM STREQUAL "arm")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=hard -mlittle-endian -munaligned-access")
	endif(D6R_PLATFORM STREQUAL "x86")
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
file(GLOB_RECURSE D6R_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "source/*.cpp" "source/*.h")

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
install(DIRECTORY resources/data DESTINATION data)
install(DIRECTORY resources/levels DESTINATION levels)
install(DIRECTORY resources/sound DESTINATION sound)
install(DIRECTORY resources/textures DESTINATION textures)
