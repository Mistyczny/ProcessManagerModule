cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(ProcessManagerModule)
enable_testing()

SET(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.71.0 COMPONENTS system filesystem log REQUIRED)
find_package(Protobuf REQUIRED)
find_package(GTest CONFIG)
find_package(spdlog REQUIRED)
find_package(Catch2 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

set(WatchdogModuleProtocolDir ${CMAKE_BINARY_DIR}/Protocols/watchdog-module-protocol)
set(ServiceModuleProtocolDirectory ${CMAKE_BINARY_DIR}/Protocols/service-module-protocol)

set(artifactoryDownloader ${CMAKE_SOURCE_DIR}/Toolkits/artifactory_downloader.sh)

set(MongoCxxIncludes /usr/local/include/mongocxx/v_noabi /usr/local/include/bsoncxx/v_noabi)

add_subdirectory(Protocols)
add_subdirectory(Source)
add_subdirectory(Tests)