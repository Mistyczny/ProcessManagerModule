protobuf_generate_cpp(PROTO_GPS_SRC PROTO_GPS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/GPS.proto
)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ModuleTwoTestTask.cpp
    ${PROTO_GPS_SRC}
    ${PROTO_GPS_HEADERS}
)

add_executable(ExampleModuleTwoTest ${SOURCE})
target_link_libraries(ExampleModuleTwoTest
        PUBLIC
    pthread
    Module
)

target_include_directories(ExampleModuleTwoTest
        PUBLIC
    ${CMAKE_SOURCE_DIR}/Source/include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/Source
)