project("emulationstation")

# Add all
file(GLOB_RECURSE ES_HEADERS src/*.h)
file(GLOB_RECURSE ES_SOURCES src/*.cpp src/*.c)

# Filter out paho library
list(FILTER ES_SOURCES EXCLUDE REGEX ".+/paho/.*")
list(FILTER ES_HEADERS EXCLUDE REGEX ".+/paho/.*")

# Add paho library manually
list(APPEND ES_SOURCES
        # Paho C
        src/mqtt/paho/c/MQTTTime.c
        src/mqtt/paho/c/MQTTProtocolClient.c
        src/mqtt/paho/c/Clients.c
        src/mqtt/paho/c/utf-8.c
        src/mqtt/paho/c/MQTTPacket.c
        src/mqtt/paho/c/MQTTPacketOut.c
        src/mqtt/paho/c/Messages.c
        src/mqtt/paho/c/Tree.c
        src/mqtt/paho/c/Socket.c
        src/mqtt/paho/c/Log.c
        src/mqtt/paho/c/MQTTPersistence.c
        src/mqtt/paho/c/Thread.c
        src/mqtt/paho/c/MQTTProtocolOut.c
        src/mqtt/paho/c/MQTTPersistenceDefault.c
        src/mqtt/paho/c/SocketBuffer.c
        src/mqtt/paho/c/LinkedList.c
        src/mqtt/paho/c/MQTTProperties.c
        src/mqtt/paho/c/MQTTReasonCodes.c
        src/mqtt/paho/c/Base64.c
        src/mqtt/paho/c/SHA1.c
        src/mqtt/paho/c/WebSocket.c
        src/mqtt/paho/c/MQTTAsync.c

        # Paho cpp
        src/mqtt/paho/cpp/async_client.cpp
        src/mqtt/paho/cpp/client.cpp
        src/mqtt/paho/cpp/connect_options.cpp
        src/mqtt/paho/cpp/disconnect_options.cpp
        src/mqtt/paho/cpp/iclient_persistence.cpp
        src/mqtt/paho/cpp/message.cpp
        src/mqtt/paho/cpp/properties.cpp
        src/mqtt/paho/cpp/response_options.cpp
        src/mqtt/paho/cpp/ssl_options.cpp
        src/mqtt/paho/cpp/string_collection.cpp
        src/mqtt/paho/cpp/subscribe_options.cpp
        src/mqtt/paho/cpp/token.cpp
        src/mqtt/paho/cpp/topic.cpp
        src/mqtt/paho/cpp/will_options.cpp
        )
list(APPEND ES_HEADERS
        # Paho C
        src/mqtt/paho/c/MQTTTime.h
        src/mqtt/paho/c/MQTTProtocolClient.h
        src/mqtt/paho/c/Clients.h
        src/mqtt/paho/c/utf-8.h
        src/mqtt/paho/c/MQTTPacket.h
        src/mqtt/paho/c/MQTTPacketOut.h
        src/mqtt/paho/c/Messages.h
        src/mqtt/paho/c/Tree.h
        src/mqtt/paho/c/Socket.h
        src/mqtt/paho/c/Log.h
        src/mqtt/paho/c/MQTTPersistence.h
        src/mqtt/paho/c/Thread.h
        src/mqtt/paho/c/MQTTProtocolOut.h
        src/mqtt/paho/c/MQTTPersistenceDefault.h
        src/mqtt/paho/c/SocketBuffer.h
        src/mqtt/paho/c/LinkedList.h
        src/mqtt/paho/c/MQTTProperties.h
        src/mqtt/paho/c/MQTTReasonCodes.h
        src/mqtt/paho/c/Base64.h
        src/mqtt/paho/c/SHA1.h
        src/mqtt/paho/c/WebSocket.h
        src/mqtt/paho/c/MQTTAsync.h

        # Paho cpp
        src/mqtt/paho/cpp/async_client.h
        src/mqtt/paho/cpp/buffer_ref.h
        src/mqtt/paho/cpp/buffer_view.h
        src/mqtt/paho/cpp/callback.h
        src/mqtt/paho/cpp/client.h
        src/mqtt/paho/cpp/connect_options.h
        src/mqtt/paho/cpp/delivery_token.h
        src/mqtt/paho/cpp/disconnect_options.h
        src/mqtt/paho/cpp/exception.h
        src/mqtt/paho/cpp/iaction_listener.h
        src/mqtt/paho/cpp/iasync_client.h
        src/mqtt/paho/cpp/iclient_persistence.h
        src/mqtt/paho/cpp/message.h
        src/mqtt/paho/cpp/properties.h
        src/mqtt/paho/cpp/response_options.h
        src/mqtt/paho/cpp/server_response.h
        src/mqtt/paho/cpp/ssl_options.h
        src/mqtt/paho/cpp/string_collection.h
        src/mqtt/paho/cpp/subscribe_options.h
        src/mqtt/paho/cpp/thread_queue.h
        src/mqtt/paho/cpp/token.h
        src/mqtt/paho/cpp/topic.h
        src/mqtt/paho/cpp/types.h
        src/mqtt/paho/cpp/will_options.h
        )

#-------------------------------------------------------------------------------
# define target
include_directories(${COMMON_INCLUDE_DIRS} src src/mqtt/paho/c src/mqtt/paho/cpp)
add_executable(emulationstation ${ES_SOURCES} ${ES_HEADERS})
target_link_libraries(emulationstation ${COMMON_LIBRARIES} es-core resolv blkid)

#-------------------------------------------------------------------------------
# set up CPack install stuff so `make install` does something useful

#
install(TARGETS emulationstation
        RUNTIME
        DESTINATION bin)

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A flexible graphical emulator front-end")
SET(CPACK_PACKAGE_DESCRIPTION "EmulationStation is a flexible, graphical front-end designed for keyboardless navigation of your multi-platform retro game collection.")

SET(CPACK_RESOURCE_FILE LICENSE "LICENSE.md")
SET(CPACK_RESOURCE_FILE README "README.md")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alec Lofquist <allofquist@yahoo.com>")
SET(CPACK_DEBIAN_PACKAGE_SECTION "misc")
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libsdl2-2.0-0, libsdl2-mixer-2.0-0, libfreeimage3, libfreetype6, libcurl3, libasound2")
SET(CPACK_DEBIAN_PACKAGE_BUILDS_DEPENDS "debhelper (>= 8.0.0), cmake, g++ (>= 4.8), libsdl2-dev, libsdl2-mixer-dev, libfreeimage-dev, libfreetype6-dev, libcurl4-openssl-dev, libasound2-dev, libgl1-mesa-dev")

SET(CPACK_PACKAGE_VENDOR "emulationstation.org")
SET(CPACK_PACKAGE_VERSION "2.0.0~rc1")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "emulationstation_${CMAKE_PACKAGE_VERSION}")
SET(CPACK_PACKAGE_EXECUTABLES "emulationstation" "emulationstation")

SET(CPACK_GENERATOR "TGZ;DEB")

INCLUDE(CPack)

