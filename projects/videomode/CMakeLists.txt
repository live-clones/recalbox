cmake_minimum_required(VERSION 3.16)
project(videomode)

set(CMAKE_CXX_STANDARD 14)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages)

find_package(SDL2 REQUIRED)

set(COMMON_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
set(COMMON_LIBRARIES ${SDL2_LIBRARY})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -Wno-attributes -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -DDEBUG -Wswitch-enum -Wswitch -O0 -g -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RECALBOX_PRODUCTION_BUILD} -Wno-attributes -O3 -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3 -Wall -Wextra -ffunction-sections -fdata-sections -Wl,--gc-sections")
endif()


include_directories(${COMMON_INCLUDE_DIRS})
add_executable(videomode main.cpp)
target_link_libraries(videomode ${COMMON_LIBRARIES})

install(TARGETS videomode RUNTIME DESTINATION bin)