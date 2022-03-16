cmake_minimum_required(VERSION 3.16)
project(check-resolutions)

set(CMAKE_CXX_STANDARD 14)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages)

option(USE_DRMKMS "Use DRM/KMS backend." ON)

if (USE_DRMKMS)
    add_definitions("-DUSE_KMSDRM=1")
    execute_process(COMMAND pkg-config libdrm --cflags OUTPUT_VARIABLE LIBDRM_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND pkg-config libdrm --libs OUTPUT_VARIABLE LIBDRM_LIBRARY OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

find_package(SDL2 REQUIRED)

set(COMMON_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
set(COMMON_LIBRARIES ${SDL2_LIBRARY} ${LIBDRM_LIBRARY})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBDRM_INCLUDE} -DDEBUG -Wno-attributes -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -DDEBUG -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBDRM_INCLUDE} -Wno-attributes -O3 -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3 -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
endif()

add_executable(check-resolutions
        main.cpp
        Resolutions.cpp
        ResolutionAdapter.cpp
        RootFolders.cpp
        utils/datetime/DateTime.cpp
        utils/Log.cpp
        utils/IniFile.cpp
        utils/Strings.cpp
        utils/Files.cpp
        utils/storage/HashMap.h
        utils/os/fs/Path.cpp
)
target_include_directories(check-resolutions PUBLIC ${CMAKE_SOURCE_DIR} ${COMMON_INCLUDE_DIRS})
target_link_libraries(check-resolutions ${COMMON_LIBRARIES} ${LIBDRM_LIBRARY})

install(TARGETS check-resolutions RUNTIME DESTINATION bin)