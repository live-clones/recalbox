set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPURE_BIOS_ONLY")

message("Flags: ${CMAKE_CXX_FLAGS}")

# Source code
file(GLOB_RECURSE SOURCES
        main.cpp
        Parameters.cpp
        Parameters.h
        server/IRouter.h
        server/handlers/RequestHandler.cpp
        server/handlers/RequestHandler.h
        server/handlers/RequestHandlerTools.cpp
        server/handlers/RequestHandlerTools.h
        server/handlers/Mime.cpp
        server/handlers/Mime.h
        server/handlers/Validator.cpp
        server/handlers/Validator.h
        server/Server.cpp
        server/Server.h
        server/handlers/providers/SysInfos.cpp
        server/handlers/providers/SysInfos.h
        server/handlers/providers/EmulationStationWatcher.cpp
        server/handlers/providers/EmulationStationWatcher.h
        server/handlers/providers/FakeInternationalizer.cpp
        server/handlers/providers/FakeInternationalizer.h
)

# ES dependencies
file(GLOB_RECURSE DEPENDENCIES
    ../es-core/src/utils/Strings.cpp
    ../es-core/src/utils/Files.cpp
    ../es-core/src/utils/os/fs/Path.cpp
    ../es-core/src/utils/IniFile.cpp
    ../es-core/src/utils/Log.cpp
    ../es-core/src/utils/Zip.cpp
    ../es-core/src/utils/os/system/Thread.cpp
    ../es-core/src/utils/os/system/Mutex.cpp
    ../es-core/src/utils/hash/Md5.cpp
    ../es-core/src/utils/datetime/DateTime.cpp
    ../es-core/src/utils/datetime/TimeSpan.cpp
    ../es-core/src/utils/storage/*.h
    ../es-core/src/RootFolders.cpp
    ../es-app/src/bios/*.cpp
    ../es-app/src/emulators/EmulatorList.cpp
    ../es-app/src/systems/SystemDescriptor.cpp
    ../es-app/src/systems/SystemDeserializer.cpp
    ../es-app/src/systems/SystemDeserializer.h
    ../es-app/src/systems/PlatformId.cpp
    ../es-app/src/systems/PlatformId.h
    ../es-app/src/mqtt/MqttClient.cpp
    ../es-app/src/mqtt/MqttClient.cpp
    ../es-app/src/LibretroRatio.cpp
    ../es-app/src/LibretroRatio.h

        # Paho C
    ../es-app/src/mqtt/paho/c/MQTTTime.c
    ../es-app/src/mqtt/paho/c/MQTTProtocolClient.c
    ../es-app/src/mqtt/paho/c/Clients.c
    ../es-app/src/mqtt/paho/c/utf-8.c
    ../es-app/src/mqtt/paho/c/MQTTPacket.c
    ../es-app/src/mqtt/paho/c/MQTTPacketOut.c
    ../es-app/src/mqtt/paho/c/Messages.c
    ../es-app/src/mqtt/paho/c/Tree.c
    ../es-app/src/mqtt/paho/c/Socket.c
    ../es-app/src/mqtt/paho/c/Log.c
    ../es-app/src/mqtt/paho/c/MQTTPersistence.c
    ../es-app/src/mqtt/paho/c/Thread.c
    ../es-app/src/mqtt/paho/c/MQTTProtocolOut.c
    ../es-app/src/mqtt/paho/c/MQTTPersistenceDefault.c
    ../es-app/src/mqtt/paho/c/SocketBuffer.c
    ../es-app/src/mqtt/paho/c/LinkedList.c
    ../es-app/src/mqtt/paho/c/MQTTProperties.c
    ../es-app/src/mqtt/paho/c/MQTTReasonCodes.c
    ../es-app/src/mqtt/paho/c/Base64.c
    ../es-app/src/mqtt/paho/c/SHA1.c
    ../es-app/src/mqtt/paho/c/WebSocket.c
    ../es-app/src/mqtt/paho/c/MQTTAsync.c
    ../es-app/src/mqtt/paho/c/MQTTTime.h
    ../es-app/src/mqtt/paho/c/MQTTProtocolClient.h
    ../es-app/src/mqtt/paho/c/Clients.h
    ../es-app/src/mqtt/paho/c/utf-8.h
    ../es-app/src/mqtt/paho/c/MQTTPacket.h
    ../es-app/src/mqtt/paho/c/MQTTPacketOut.h
    ../es-app/src/mqtt/paho/c/Messages.h
    ../es-app/src/mqtt/paho/c/Tree.h
    ../es-app/src/mqtt/paho/c/Socket.h
    ../es-app/src/mqtt/paho/c/Log.h
    ../es-app/src/mqtt/paho/c/MQTTPersistence.h
    ../es-app/src/mqtt/paho/c/Thread.h
    ../es-app/src/mqtt/paho/c/MQTTProtocolOut.h
    ../es-app/src/mqtt/paho/c/MQTTPersistenceDefault.h
    ../es-app/src/mqtt/paho/c/SocketBuffer.h
    ../es-app/src/mqtt/paho/c/LinkedList.h
    ../es-app/src/mqtt/paho/c/MQTTProperties.h
    ../es-app/src/mqtt/paho/c/MQTTReasonCodes.h
    ../es-app/src/mqtt/paho/c/Base64.h
    ../es-app/src/mqtt/paho/c/SHA1.h
    ../es-app/src/mqtt/paho/c/WebSocket.h
    ../es-app/src/mqtt/paho/c/MQTTAsync.h

    # Paho cpp
    ../es-app/src/mqtt/paho/cpp/async_client.h
    ../es-app/src/mqtt/paho/cpp/buffer_ref.h
    ../es-app/src/mqtt/paho/cpp/buffer_view.h
    ../es-app/src/mqtt/paho/cpp/callback.h
    ../es-app/src/mqtt/paho/cpp/client.h
    ../es-app/src/mqtt/paho/cpp/connect_options.h
    ../es-app/src/mqtt/paho/cpp/delivery_token.h
    ../es-app/src/mqtt/paho/cpp/disconnect_options.h
    ../es-app/src/mqtt/paho/cpp/exception.h
    ../es-app/src/mqtt/paho/cpp/iaction_listener.h
    ../es-app/src/mqtt/paho/cpp/iasync_client.h
    ../es-app/src/mqtt/paho/cpp/iclient_persistence.h
    ../es-app/src/mqtt/paho/cpp/message.h
    ../es-app/src/mqtt/paho/cpp/properties.h
    ../es-app/src/mqtt/paho/cpp/response_options.h
    ../es-app/src/mqtt/paho/cpp/server_response.h
    ../es-app/src/mqtt/paho/cpp/ssl_options.h
    ../es-app/src/mqtt/paho/cpp/string_collection.h
    ../es-app/src/mqtt/paho/cpp/subscribe_options.h
    ../es-app/src/mqtt/paho/cpp/thread_queue.h
    ../es-app/src/mqtt/paho/cpp/token.h
    ../es-app/src/mqtt/paho/cpp/topic.h
    ../es-app/src/mqtt/paho/cpp/types.h
    ../es-app/src/mqtt/paho/cpp/will_options.h
    ../es-app/src/mqtt/paho/cpp/async_client.cpp
    ../es-app/src/mqtt/paho/cpp/client.cpp
    ../es-app/src/mqtt/paho/cpp/connect_options.cpp
    ../es-app/src/mqtt/paho/cpp/disconnect_options.cpp
    ../es-app/src/mqtt/paho/cpp/iclient_persistence.cpp
    ../es-app/src/mqtt/paho/cpp/message.cpp
    ../es-app/src/mqtt/paho/cpp/properties.cpp
    ../es-app/src/mqtt/paho/cpp/response_options.cpp
    ../es-app/src/mqtt/paho/cpp/ssl_options.cpp
    ../es-app/src/mqtt/paho/cpp/string_collection.cpp
    ../es-app/src/mqtt/paho/cpp/subscribe_options.cpp
    ../es-app/src/mqtt/paho/cpp/token.cpp
    ../es-app/src/mqtt/paho/cpp/topic.cpp
    ../es-app/src/mqtt/paho/cpp/will_options.cpp
)

include_directories(
        .
        ../es-core/src
        ../es-app/src
        ../external
        pistache/include
)

add_executable(
        webapi-server
        ${SOURCES}
        ${DEPENDENCIES}
)

add_subdirectory(pistache/src)

target_link_libraries(webapi-server pistache_static zip pugixml)

#-------------------------------------------------------------------------------
# set up install stuff so `make install` does something useful
install(TARGETS webapi-server
        RUNTIME
        DESTINATION bin)