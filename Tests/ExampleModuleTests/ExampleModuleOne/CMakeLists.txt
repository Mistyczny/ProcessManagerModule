
protobuf_generate_cpp(PROTO_GPS_SRC PROTO_GPS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/GPS.proto
)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ModuleOneTestTask.cpp
    ${PROTO_GPS_SRC}
    ${PROTO_GPS_HEADERS}
)

add_executable(ExampleModuleOneTest ${SOURCE})
target_link_libraries(ExampleModuleOneTest
        PUBLIC
    pthread
    Module
)

target_include_directories(ExampleModuleOneTest
        PUBLIC
    ${CMAKE_SOURCE_DIR}/Source/include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/Source
)