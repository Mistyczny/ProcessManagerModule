#include "ModuleResponseConnectHandler.hpp"
#include "Logging.hpp"
namespace Module {

ResponseConnectHandler::ResponseConnectHandler(uint32_t& sequenceCode, WatchdogConnectionState& watchdogConnectionState)
    : sequenceCode{sequenceCode}, watchdogConnectionState{watchdogConnectionState} {}

void ResponseConnectHandler::handleResponse(std::unique_ptr<Communication::Message<WatchdogModule::Operation>> receivedMessage) {
    if (!receivedMessage) {
        Log::error("Passed received message is nullptr");
    } else if (this->watchdogConnectionState.getConnectionState() == ConnectionState::Connected) {
        Log::error("Module is already connected");
    } else {
        auto& [header, body] = *receivedMessage;
        connectResponseData.ParseFromString(body);
        this->handleReceivedMessage();
    }
}

void ResponseConnectHandler::handleReceivedMessage() {
    ConnectionState newConnectionState{ConnectionState::NotConnected};
    switch (this->connectResponseData.responsecode()) {
    case WatchdogModule::ConnectResponseData::Success:
        newConnectionState = this->onResponseCodeSuccess();
        break;
    case WatchdogModule::ConnectResponseData::InvalidConnectionState:
        Log::error("Module is in invalid connection state");
        break;
    case WatchdogModule::ConnectResponseData::NotModuleIdentifier:
        Log::error("Provided identifier is not qualified as module identifier");
        break;
    case WatchdogModule::ConnectResponseData::ModuleNotExists:
        Log::error("Module with provided identifier is not registered in database");
        break;
    default:
        Log::error("Received invalid connect response code");
        break;
    }
    this->watchdogConnectionState.setConnectionStateAndNotifyAll(newConnectionState);
}

ConnectionState ResponseConnectHandler::onResponseCodeSuccess() {
    ConnectionState connectionState{ConnectionState::NotConnected};
    if (connectResponseData.has_sequencecode()) {
        this->sequenceCode = connectResponseData.sequencecode();
        connectionState = ConnectionState::Connected;
    } else {
        Log::critical("Received connect response that doesn't have sequence code");
    }
    return connectionState;
}

} // namespace Module