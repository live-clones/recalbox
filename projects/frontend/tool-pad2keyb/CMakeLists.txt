# Test code
file(GLOB_RECURSE SOURCES
        main.cpp
        ../es-app/src/padtokeyboard/keyboard/VirtualKeyboard.cpp
        ../es-app/src/padtokeyboard/pad/MappingConfiguration.cpp
        ../es-app/src/padtokeyboard/pad/PadConfiguration.cpp
        ../es-app/src/padtokeyboard/pad/Pad.cpp
)

# Tested code & dependencies
file(GLOB_RECURSE DEPENDENCIES
        ../es-core/src/utils/Strings.cpp
        ../es-core/src/utils/Files.cpp
        ../es-core/src/utils/os/fs/Path.cpp
        ../es-core/src/utils/Log.cpp
        ../es-core/src/utils/datetime/DateTime.cpp
        ../es-core/src/RootFolders.cpp
        ../es-core/src/input/InputDevice.cpp
        ../es-core/src/input/InputEvent.cpp
        ../es-core/src/input/Sdl2Extentions.cpp
)

# Set up compiler flags for GCC
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes -Os -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Os -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
endif()

include_directories(
        .
        ../es-core/src
        ../es-app/src
        ../external
)

add_executable(
        pad2keyb
        ${SOURCES}
        ${DEPENDENCIES}
)

target_link_libraries(pad2keyb pugixml ${SDL2_LIBRARY})
