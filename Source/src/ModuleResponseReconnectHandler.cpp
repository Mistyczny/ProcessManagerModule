#include "ModuleResponseReconnectHandler.hpp"
#include "Logging.hpp"

namespace Module {

ResponseReconnectHandler::ResponseReconnectHandler(uint32_t& sequenceCode, WatchdogConnectionState& watchdogConnectionState)
    : sequenceCode{sequenceCode}, watchdogConnectionState{watchdogConnectionState} {}

void ResponseReconnectHandler::handleResponse(std::unique_ptr<Communication::Message<WatchdogModule::Operation>> receivedMessage) {
    if (!receivedMessage) {
        Log::error("Passed received message is nullptr");
    } else if (this->watchdogConnectionState.getConnectionState() != ConnectionState::NotConnected) {
        Log::error("Module is in invalid state");
    } else {
        auto& [header, body] = *receivedMessage;
        reconnectResponseData.ParseFromString(body);
        // this->handleReceivedMessage();
    }
}

} // namespace Module