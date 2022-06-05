project("core")

# Add all
file(GLOB_RECURSE CORE_HEADERS src/*.h)
file(GLOB_RECURSE CORE_SOURCES src/*.cpp)
# Filter out
list(FILTER CORE_SOURCES EXCLUDE REGEX ".+/alsa/.*")
list(FILTER CORE_HEADERS EXCLUDE REGEX ".+/alsa/.*")

# Add resources
file(GLOB_RECURSE EMBEDDED_ASSET_SOURCES ${emulationstation-all_SOURCE_DIR}/data/*.cpp)

# Append both sources & resources
list(APPEND CORE_SOURCES ${EMBEDDED_ASSET_SOURCES})

set(FFMPEGLIBS avcodec avformat swresample avutil swscale avdevice)

include_directories(${COMMON_INCLUDE_DIRS})
add_library(es-core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(es-core ${COMMON_LIBRARIES} ${FFMPEGLIBS} resolv curl zip procps)
