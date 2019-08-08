cmake_minimum_required(VERSION 2.8)

project(emulationstation-all)

#-------------------------------------------------------------------------------
#set version number
if (DEFINED ENV{RECALBOX_VERSION})
    set(PROGRAM_VERSION_STRING $ENV{RECALBOX_VERSION})
elseif (DEFINED ENV{RECALBOX_VERSION_LABEL})
    set(PROGRAM_VERSION_STRING $ENV{RECALBOX_VERSION_LABEL})
else()
    set(PROGRAM_VERSION_STRING "dev")
endif()

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/es-app/src/EmulationStation.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/es-app/src/EmulationStation.h"
)


#-------------------------------------------------------------------------------
#add local find scripts to CMAKE path
LIST(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages
)

#-------------------------------------------------------------------------------
#set up OpenGL system variable
set(GLSystem "Desktop OpenGL" CACHE STRING "The OpenGL system to be used")
set_property(CACHE GLSystem PROPERTY STRINGS "Desktop OpenGL" "OpenGL ES")

#-------------------------------------------------------------------------------
# finding GL compatible library
MESSAGE("Looking for OpenGL or OpenGLES")
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENGL gl)
#pkg_check_modules(EGL egl)
pkg_check_modules(OPENGLES glesv2)
if(OPENGL_FOUND)
    MESSAGE("OpenGL found ${OPENGL_LIBRARIES}")
    find_package(OpenGL REQUIRED)
elseif(OPENGLES_FOUND)
    MESSAGE("OpenGLES found ${OPENGLES_LIBRARIES}")
    find_package(OpenGLES REQUIRED)
    set(GLSystem "OpenGL ES")
else()
    MESSAGE(FATAL_ERROR "OpenGL or OpenGLES required!")
endif()

pkg_check_modules(BCMHOST bcm_host)

#-------------------------------------------------------------------------------
# finding necessary packages
find_package(Freetype REQUIRED)
find_package(FreeImage REQUIRED)
find_package(SDL2MIXER REQUIRED)
find_package(SDL2 REQUIRED) 
find_package(Boost REQUIRED COMPONENTS system filesystem date_time regex locale thread)
find_package(Eigen3 REQUIRED)
find_package(CURL REQUIRED)

#add ALSA for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(ALSA REQUIRED)
endif()

#-------------------------------------------------------------------------------
#set up compiler flags and excutable names
if(BCMHOST_FOUND)
    add_definitions(-D_RPI_)
endif()

#-------------------------------------------------------------------------------

if(CMAKE_COMPILER_IS_GNUCXX)
    #check for G++ 4.7+
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE G++_VERSION)
    if (G++_VERSION VERSION_LESS 4.7)
            message(SEND_ERROR "You need at least G++ 4.7 to compile EmulationStation!")
    endif()

    #set up compiler flags for GCC
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections") #support C++11 for std::, optimize
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes -O3 -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections") #support C++11 for std::, optimize
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3 -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")  #-s = strip binary
	endif()
endif()

if(APPLE AND NOT CMAKE_COMPILER_IS_GNUCXX)
    #set up compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes -O3") #support C++11 for std::, optimize
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3")  #-s = strip binary

    #set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
endif()

if(${GLSystem} MATCHES "Desktop OpenGL")
    add_definitions(-DUSE_OPENGL_DESKTOP)
else()
    add_definitions(-DUSE_OPENGL_ES)
endif()

#-------------------------------------------------------------------------------
#add include directories
set(COMMON_INCLUDE_DIRS
    # OSX seems to have a conflicting platform.h somewhere and fails to find
    # getHomePath, so let use our includes paths first.
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/es-app/src
    ${CMAKE_CURRENT_SOURCE_DIR}/es-core/src
    ${FREETYPE_INCLUDE_DIRS}
    ${FreeImage_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIR}
)

#add ALSA for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    LIST(APPEND COMMON_INCLUDE_DIRS
        ${ALSA_INCLUDE_DIRS}
    )
endif()

if(${GLSystem} MATCHES "Desktop OpenGL")
    LIST(APPEND COMMON_INCLUDE_DIRS
	${OPENGL_INCLUDE_DIR}
    )
else()
    LIST(APPEND COMMON_INCLUDE_DIRS
	${OPENGLES_INCLUDE_DIR}
    )
endif()

#-------------------------------------------------------------------------------
#define libraries and directories
link_directories(${Boost_LIBRARY_DIRS})

set(COMMON_LIBRARIES
    ${Boost_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FreeImage_LIBRARIES}
	${SDL2_LIBRARY}
	${SDLMIXER_LIBRARY}
    ${CURL_LIBRARIES}
    pugixml
    nanosvg
)

#add ALSA for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    LIST(APPEND COMMON_LIBRARIES
        ${ALSA_LIBRARY}
    )
endif()

if(MSVC)
    LIST(APPEND COMMON_LIBRARIES
	winmm
    )
endif()
if(${GLSystem} MATCHES "Desktop OpenGL")
    LIST(APPEND COMMON_LIBRARIES
	${OPENGL_LIBRARIES}
    )
else()
    LIST(APPEND COMMON_LIBRARIES
	EGL
	${OPENGLES_LIBRARIES}
    )
endif()

#-------------------------------------------------------------------------------
# set up build directories
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)


#-------------------------------------------------------------------------------
# add each component

add_subdirectory("external")
add_subdirectory("es-core")
add_subdirectory("es-app")


# i18n
find_program (MSGFMT_EXECUTABLE msgfmt)
find_program (MSGMERGE_EXECUTABLE msgmerge)
find_program (XGETTEXT_EXECUTABLE xgettext)
if(MSGFMT_EXECUTABLE AND MSGMERGE_EXECUTABLE AND XGETTEXT_EXECUTABLE)
	message (STATUS "Native language support enabled.")
	add_subdirectory (locale)
endif()
