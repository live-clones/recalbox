# Test code
file(GLOB SOURCES main.cpp)

# Tested code & dependencies
file(GLOB_RECURSE DEPENDENCIES
        ../es-core/src/utils/hash/Md5.cpp
        ../es-core/src/utils/Zip.cpp
        ../es-core/src/utils/Strings.cpp
        ../es-core/src/utils/os/fs/Path.cpp
        )

include_directories(
        .
        ../es-core/src
        ../external
)

add_executable(
        md5zip
        ${SOURCES}
        ${DEPENDENCIES}
)

target_link_libraries(md5zip zip)