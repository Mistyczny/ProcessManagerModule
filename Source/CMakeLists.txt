project(Module)

set(SOURCES 
    src/ModuleMain.cpp
    src/ModuleBase.cpp
    src/Event.cpp
    src/TimersCache.cpp
    src/TimersThreadMain.cpp
    src/Timer.cpp
    src/ModuleTimersManager.cpp
    src/ModuleConnectionManager.cpp
    src/ModuleSettings.cpp
    src/ApplicationDetails.cpp
    src/ModuleConnection.cpp
    src/ModuleResponsePingHandler.cpp
    src/Types.cpp
    src/ModuleGlobals.cpp
    src/ModuleResponseConnectHandler.cpp
    src/ModuleWatchdogConnectionState.cpp
    src/ModuleResponseReconnectHandler.cpp
        src/ModuleServer.cpp
)

add_library(ModuleStatic STATIC ${SOURCES})
target_link_libraries(ModuleStatic
        PUBLIC
    spdlog
        ${Boost_LIBRARIES}
    WatchdogModuleProto
        ServiceModuleProto
    ${PROTOBUF_LIBRARY}
)
target_include_directories(ModuleStatic 
        PRIVATE
        ${BOOST_ROOT}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/Protocols
)