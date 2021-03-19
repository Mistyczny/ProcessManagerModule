project(ModuleServerTest)

set(ModuleServerSource
    ${CMAKE_SOURCE_DIR}/Source/src/ModuleServer.cpp
    ${CMAKE_SOURCE_DIR}/Source/src/Timer.cpp
    ${CMAKE_SOURCE_DIR}/Source/src/TimersCache.cpp
    ${CMAKE_SOURCE_DIR}/Source/src/ModuleGlobals.cpp
    ${CMAKE_SOURCE_DIR}/Source/src/Types.cpp
    ${CMAKE_SOURCE_DIR}/Source/src//ModuleConfiguration.cpp
)

add_executable(ModuleServerTests ./ModuleServerTests.cpp ${ModuleServerSource})
target_link_libraries(ModuleServerTests
        PRIVATE
    pthread
    catchTestMain
    ServiceModuleProto
    ${PROTOBUF_LIBRARY}
    ${Boost_LIBRARIES}
    GTest::gtest
    GTest::gmock
)
target_include_directories(ModuleServerTests
        PRIVATE
    ${BOOST_ROOT}
    ${CMAKE_SOURCE_DIR}/Source/include/
    ${CMAKE_BINARY_DIR}/Protocols
)

add_test(NAME ModuleServerTests COMMAND ModuleServerTests)