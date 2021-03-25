
find_package(Catch2 REQUIRED)
find_package(GTest CONFIG)

add_subdirectory(ModuleServerTests)
add_subdirectory(ModuleWatchdogMessageHandlersTests)
add_subdirectory(ExampleModuleTests)

add_library(catchTestMain STATIC CatchTestsMain.cpp)
target_link_libraries(catchTestMain PUBLIC Catch2::Catch2)