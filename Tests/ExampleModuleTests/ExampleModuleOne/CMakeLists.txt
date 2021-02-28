project(ExampleModuleOne)

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/ModuleOneTestTask.cpp)

add_executable(ExampleModuleOneTest ${SOURCE})
target_link_libraries(ExampleModuleOneTest
        PUBLIC
        pthread
        ModuleStatic
        )

target_include_directories(ExampleModuleOneTest
        PUBLIC
        ${CMAKE_SOURCE_DIR}/Source/include
        )