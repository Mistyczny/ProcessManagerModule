#include "ModuleConnection.hpp"
#include "Logging.hpp"
#include "ModuleResponseConnectHandler.hpp"
#include "ModuleResponsePingHandler.hpp"
#include "ModuleResponseReconnectHandler.hpp"
#include "WatchdogModule.pb.h"

namespace Module {

ModuleWatchdogConnection::ModuleWatchdogConnection(boost::asio::io_context& ioContext, WatchdogConnectionState& watchdogConnectionState)
    : Connection::TcpConnection<WatchdogModule::Operation>(ioContext), watchdogConnectionState{watchdogConnectionState} {}

ModuleWatchdogConnection::~ModuleWatchdogConnection() {
    Log::info("ModuleWatchdogConnection::~ModuleWatchdogConnection Connection closed");
}

void ModuleWatchdogConnection::handleReceivedMessage(std::unique_ptr<Communication::Message<WatchdogModule::Operation>> receivedMessage) {
    if (!receivedMessage) {
        Log::error("handleReceivedMessage: receivedMessage is nullptr");
    } else {
        Log::info("ModuleWatchdogConnection::handleReceivedMessage in");
        auto& [messageHeader, messageBody] = *receivedMessage;
        auto responseCreator = this->getResponseHandler(messageHeader.operationCode);
        if (responseCreator) {
            Log::info("ModuleWatchdogConnection::handleReceivedMessage handle response");
            responseCreator->handleResponse(std::move(receivedMessage));
        }
    }
}

void ModuleWatchdogConnection::sendConnectRequest() {
    Communication::Message<WatchdogModule::Operation> connectMessage{};
    WatchdogModule::ConnectRequestData requestData{};
    requestData.set_identifier(Globals::moduleIdentifier);
    requestData.SerializeToString(&connectMessage.body);
    connectMessage.header.operationCode = WatchdogModule::Operation::ConnectRequest;
    connectMessage.header.size = connectMessage.body.size();
    this->sendMessage(connectMessage);
}

void ModuleWatchdogConnection::sendPing() {
    Communication::Message<WatchdogModule::Operation> pingMessage{};
    WatchdogModule::PingRequestData pingRequest{};
    pingRequest.set_sequencecode(this->sequenceCode);
    pingRequest.SerializeToString(&pingMessage.body);
    pingMessage.header.operationCode = WatchdogModule::Operation::PingRequest;
    pingMessage.header.size = pingMessage.body.size();
    this->sendMessage(pingMessage);
}

void ModuleWatchdogConnection::onTimerExpiration() { this->sendPing(); }

void ModuleWatchdogConnection::disconnect() {
    if (this->socket->is_open()) {
        this->socket->close();
    }
    auto currentState = this->watchdogConnectionState.getConnectionState();
    if (currentState == ConnectionState::Connected) {
        this->watchdogConnectionState.setConnectionStateAndNotifyAll(ConnectionState::NotConnected);
    }
}

std::unique_ptr<ModuleResponseHandler> ModuleWatchdogConnection::getResponseHandler(WatchdogModule::Operation& operationCode) {
    std::unique_ptr<ModuleResponseHandler> responseHandler{nullptr};
    Log::info("Received :" + std::to_string(static_cast<int>(operationCode)) + " message operation code");
    auto startPingTimer = std::bind([&]() { this->setTimerExpiration(3000); });
    switch (operationCode) {
    case WatchdogModule::Operation::ConnectResponse:
        responseHandler = std::make_unique<ResponseConnectHandler>(this->sequenceCode, this->watchdogConnectionState);
        break;
    case WatchdogModule::Operation::PingResponse:
        responseHandler = std::make_unique<ResponsePingHandler>(this->sequenceCode, std::move(startPingTimer));
        break;
    case WatchdogModule::Operation::ReconnectResponse:
        responseHandler = std::make_unique<ResponseReconnectHandler>(this->sequenceCode, this->watchdogConnectionState);
        break;
    default:
        Log::error("Received unknown operation code: " + std::to_string(static_cast<int>(operationCode)));
        break;
    }
    return responseHandler;
}

void ModuleWatchdogConnection::sendShutdownRequest() {
    Communication::Message<WatchdogModule::Operation> shutdownMessage{};
    WatchdogModule::ShutdownRequestData shutdownRequestData{};
    Log::error("Sending module identifier: " + std::to_string(Globals::moduleIdentifier));
    shutdownRequestData.set_identifier(Globals::moduleIdentifier);
    shutdownRequestData.SerializeToString(&shutdownMessage.body);
    shutdownMessage.header.operationCode = WatchdogModule::Operation::ShutdownRequest;
    shutdownMessage.header.size = shutdownMessage.body.size();

    this->sendMessage(shutdownMessage);
}

} // namespace Module