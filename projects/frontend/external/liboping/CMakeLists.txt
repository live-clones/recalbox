project("liboping")

set(OPING_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/oping.h
        )

set(OPING_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/oping.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/liboping.c
        )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_library(liboping STATIC ${OPING_SOURCES} ${OPING_HEADERS})
