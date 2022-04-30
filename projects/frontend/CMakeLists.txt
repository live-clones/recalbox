cmake_minimum_required(VERSION 2.8)

project(emulationstation-all)

#-------------------------------------------------------------------------------
#set version number
if (DEFINED ENV{RECALBOX_VERSION})
    set(PROGRAM_VERSION_STRING $ENV{RECALBOX_VERSION})
elseif (DEFINED ENV{RECALBOX_VERSION_LABEL})
    set(PROGRAM_VERSION_STRING $ENV{RECALBOX_VERSION_LABEL})
else()
    set(PROGRAM_VERSION_STRING "7.0-dev")
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
find_package(CURL REQUIRED)
find_package(PulseAudio REQUIRED)

#-------------------------------------------------------------------------------
#set up compiler flags and excutable names
if(BCMHOST_FOUND)
    add_definitions(-D_RPI_)
endif()

#-------------------------------------------------------------------------------
option(USE_DRMKMS "Use DRM/KMS backend." OFF)

if (USE_DRMKMS)
    add_definitions("-DUSE_KMSDRM=1")
    execute_process(COMMAND pkg-config libdrm --cflags OUTPUT_VARIABLE LIBDRM_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND pkg-config libdrm --libs OUTPUT_VARIABLE LIBDRM_LIBRARY OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

#-------------------------------------------------------------------------------

if(CMAKE_COMPILER_IS_GNUCXX)
    #check for G++ 4.7+
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE G++_VERSION)
    if (G++_VERSION VERSION_LESS 4.7)
            message(SEND_ERROR "You need at least G++ 4.7 to compile EmulationStation!")
    endif()

    if(OPTION_RECALBOX_PRODUCTION_BUILD)
        set(RECALBOX_PRODUCTION_BUILD "-D_RECALBOX_PRODUCTION_BUILD_=1")
    endif()

    #set up compiler flags for GCC
	if (CMAKE_BUILD_TYPE MATCHES Debug)
        # -fsanitize=address -fsanitize-address-use-after-scope
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBDRM_INCLUDE} -DDEBUG -Wno-attributes -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections -DPAHO_MQTT_STATIC=1 -DHIGH_PERFORMANCE=1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBDRM_INCLUDE} -std=c++11 -DDEBUG -Wno-attributes -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections") #support C++11 for std::, optimize
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -DDEBUG -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${RECALBOX_PRODUCTION_BUILD} ${LIBDRM_INCLUDE} -Wno-attributes -O3 -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections -DPAHO_MQTT_STATIC=1 -DHIGH_PERFORMANCE=1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RECALBOX_PRODUCTION_BUILD} ${LIBDRM_INCLUDE} -std=c++11 -Wno-attributes -O3 -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections") #support C++11 for std::, optimize
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3 -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")  #-s = strip binary
	endif()
endif()

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer -g")

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
    ${CURL_INCLUDE_DIR}
    ${PULSEAUDIO_INCLUDE_DIR}
)

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
#link_directories(${Boost_LIBRARY_DIRS})

set(COMMON_LIBRARIES
    #${Boost_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FreeImage_LIBRARIES}
	${SDL2_LIBRARY}
	${SDLMIXER_LIBRARY}
    ${CURL_LIBRARIES}
    ${PULSEAUDIO_LIBRARY}
    ${LIBDRM_LIBRARY}
    pugixml
    nanosvg
    liboping
)

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

# pad to keyb
#add_subdirectory(tool-pad2keyb)

# Tests
add_subdirectory(tests)

# md5zip
add_subdirectory(tool-md5zip)

message(
        "=========================================================================================================\n"
        "\n"
        "C    Flags : ${CMAKE_C_FLAGS}\n"
        "C++  Flags : ${CMAKE_CXX_FLAGS}\n"
        "Link Flags : ${CMAKE_EXE_LINKER_FLAGS}\n"
        "\n"
        "=========================================================================================================\n"
)