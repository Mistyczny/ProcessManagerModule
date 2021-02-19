project(CommunicationProtocols)

include(ExternalProject)

set(ArtifactoryProtocolsPath /process-manager-local/CommunicationProtocols)

set(WatchdogModuleProtocolVersion 1.0)
set(WatchdogModuleProtocolPackage WatchdogModuleProtocol_${WatchdogModuleProtocolVersion}.tar)

set(WatchdogModuleProtocolDir ${CMAKE_CURRENT_BINARY_DIR}/watchdog-service-protocol)
ExternalProject_Add(WatchdogModuleProtocolDownload
    PREFIX
        ${CMAKE_CURRENT_BINARY_DIR}/download
    BINARY_DIR
        ${WatchdogModuleProtocolDir}
    DOWNLOAD_COMMAND
        ${artifactoryDownloader} ${WatchdogModuleProtocolPackage} . ${ArtifactoryProtocolsPath}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND
        tar xf ${CMAKE_CURRENT_BINARY_DIR}/download/src/${WatchdogModuleProtocolPackage}
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

include_directories(${PROTOBUF_INCLUDE_DIR})
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${WatchdogModuleProtocolDir}/WatchdogModule.proto)
add_library(WatchdogModuleProto ${PROTO_HEADER} ${PROTO_SRC})
add_dependencies(WatchdogModuleProto WatchdogModuleProtocolDownload)