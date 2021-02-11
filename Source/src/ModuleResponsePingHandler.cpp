#include "ModuleResponsePingHandler.hpp"

#include "Logging.hpp"
#include <utility>

namespace Module {

ResponsePingHandler::ResponsePingHandler(uint32_t& sequenceCode, std::function<void()> pingTimerSet)
    : sequenceCode{sequenceCode}, pingTimerSet{std::move(pingTimerSet)} {}

void ResponsePingHandler::handleResponse(std::unique_ptr<Communication::Message<WatchdogModule::Operation>> receivedMessage) {
    if (!receivedMessage) {
        Log::error("Passed received message is nullptr");
    } else {
        WatchdogModule::PingResponseData pingResponseData{};
        pingResponseData.ParseFromString(receivedMessage->body);
        if (pingResponseData.sequencecode() == this->sequenceCode) {
            this->pingTimerSet();
        } else {
            Log::error("Received ping response with invalid sequence code, dropping");
        }
    }
}

} // namespace Module