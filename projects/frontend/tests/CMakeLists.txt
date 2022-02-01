# Googletest framework
file(GLOB GOOGLETEST_SOURCES googletest/src/gtest-all.cc)

# Test code
file(GLOB_RECURSE TESTS_SOURCES tests/*.cpp)

# Tested code & dependencies
file(GLOB_RECURSE TESTED_PATH ../es-core/src/utils/*.cpp ../es-core/src/RootFolders.cpp)
# All tested code
set(ALL_TESTED_SOURCES ${TESTED_PATH})

find_package(SDL2 REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIR}
        ../es-core/src
        ../es-app/src
        googletest/include
        googletest
)

add_executable(
        emulationstation_test
        main.cpp
        ${GOOGLETEST_SOURCES}
        ${TESTS_SOURCES}
        ${ALL_TESTED_SOURCES}
)

target_link_libraries(emulationstation_test pthread ${SDL2_LIBRARY} curl zip procps)
