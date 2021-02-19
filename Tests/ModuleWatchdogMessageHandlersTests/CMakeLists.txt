project(ModuleMessageHandlersTests)

########################################### ModuleResponseConnectHandler ###############################################
set(ModuleResponseConnectHandlerSource  
    ${CMAKE_SOURCE_DIR}/Source/src//ModuleResponseConnectHandler.cpp
    ${CMAKE_SOURCE_DIR}/Source/src/ModuleWatchdogConnectionState.cpp
    ${CMAKE_SOURCE_DIR}/Source/src/Types.cpp
)
add_executable(ModuleResponseConnectHandlerTest ./ModuleResponseConnectHandlerTest.cpp ${ModuleResponseConnectHandlerSource})
target_link_libraries(ModuleResponseConnectHandlerTest 
        PRIVATE
    pthread
    catchTestMain
    WatchdogModuleProto
    ${PROTOBUF_LIBRARY}
)
target_include_directories(ModuleResponseConnectHandlerTest 
        PRIVATE 
    ${CMAKE_SOURCE_DIR}/Source/include/
    ${CMAKE_BINARY_DIR}/Protocols
)

########################################### ModuleResponseConnectHandler ###############################################
set(ModuleResponsePingHandlerSource 
    ${CMAKE_SOURCE_DIR}/Source/src/ModuleResponsePingHandler.cpp
    ${CMAKE_SOURCE_DIR}/Source/src/Types.cpp
)
add_executable(ModuleResponsePingHandlerTest ./ModuleResponsePingHandlerTest.cpp ${ModuleResponsePingHandlerSource})
target_link_libraries(ModuleResponsePingHandlerTest 
        PRIVATE
    pthread
    catchTestMain
    WatchdogModuleProto
    ${PROTOBUF_LIBRARY}
)
target_include_directories(ModuleResponsePingHandlerTest 
        PRIVATE 
    ${CMAKE_SOURCE_DIR}/Source/include/
    ${CMAKE_BINARY_DIR}/Protocols
)

add_test(NAME ModuleResponseConnectHandlerTest COMMAND ModuleResponseConnectHandlerTest)
add_test(NAME ModuleResponsePingHandlerTest COMMAND ModuleResponsePingHandlerTest)