cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(process-manager-module)
enable_testing()

SET(CMAKE_CXX_STANDARD 20)

set(CPACK_DEBIAN_PACKAGE_NAME @CMAKE_PROJECT_NAME@)
set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kacper Waśniowski")
set(CPACK_PACKAGE_CONTACT "Kacper Waśniowski")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

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

configure_file(Doxyfile.in Doxyfile)

add_subdirectory(Protocols)
add_subdirectory(Source)
add_subdirectory(Tests)

include(CPack)