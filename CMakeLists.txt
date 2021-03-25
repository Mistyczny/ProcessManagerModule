cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(process-manager-module)
enable_testing()

set(BUILD_TESTS False CACHE STRING "Turn on to build tests")

SET(CMAKE_CXX_STANDARD 20)

set(CPACK_DEBIAN_PACKAGE_NAME @CMAKE_PROJECT_NAME@)
set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kacper Waśniowski")
set(CPACK_PACKAGE_CONTACT "Kacper Waśniowski")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

find_package(Boost 1.71.0 COMPONENTS system filesystem log program_options REQUIRED)
find_package(Protobuf 3.14 EXACT REQUIRED)
find_package(mongocxx 3.6.1 REQUIRED)
find_package(bsoncxx 3.6.1 REQUIRED)
find_package(spdlog 1.8.2 EXACT REQUIRED)
find_package(nlohmann_json 3.9.1 EXACT REQUIRED)

set(WatchdogModuleProtocolDir ${CMAKE_BINARY_DIR}/Protocols/watchdog-module-protocol)
set(ServiceModuleProtocolDirectory ${CMAKE_BINARY_DIR}/Protocols/service-module-protocol)

set(artifactoryDownloader ${CMAKE_SOURCE_DIR}/Toolkits/artifactory_downloader.sh)

configure_file(Doxyfile.in Doxyfile)

add_subdirectory(Protocols)
add_subdirectory(Source)
if(${BUILD_TESTS})
    add_subdirectory(Tests)
endif()

include(CPack)