project(Module)

include_directories(${PROTOBUF_INCLUDE_DIR})
protobuf_generate_cpp(PROTO_WATCHDOG_MODULE_SRC PROTO_WATCHDOG_MODULE_SRC_HEADER ${WatchdogModuleProtocolDir}/WatchdogModule.proto)
protobuf_generate_cpp(PROTO_SERVICE_MODULE_SRC PROTO_SERVICE_MODULE_HEADERS ${ServiceModuleProtocolDirectory}/ServiceModule.proto)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleMain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TimersCache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TimersThreadMain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleTimersManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleConnectionManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ApplicationDetails.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleConnection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleResponsePingHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleGlobals.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleResponseConnectHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleWatchdogConnectionState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleResponseReconnectHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleConfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleConfigurationReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MongoDbEnvironment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MongoServicesCollection.cpp
    ${PROTO_WATCHDOG_MODULE_SRC}
    ${PROTO_SERVICE_MODULE_SRC}
)

add_library(Module SHARED ${SOURCES})
add_dependencies(Module
    WatchdogModuleProtocolDownload
    ServiceModuleProtocolDownload
)
target_link_libraries(Module
        PUBLIC
    spdlog
    ${Boost_LIBRARIES}
    ${PROTOBUF_LIBRARY}
    nlohmann_json::nlohmann_json
)

target_include_directories(Module
        PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/Source>"
    "$<BUILD_INTERFACE:${MongoCxxIncludes}>"
)