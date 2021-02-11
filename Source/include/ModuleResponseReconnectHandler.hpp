#pragma once
#include "ModuleResponseHandler.hpp"
#include "ModuleWatchdogConnectionState.hpp"
#include "WatchdogModule.pb.h"

namespace Module {

class ResponseReconnectHandler : public ModuleResponseHandler {
protected:
    uint32_t& sequenceCode;
    WatchdogConnectionState& watchdogConnectionState;
    WatchdogModule::ReconnectResponseData reconnectResponseData;

public:
    ResponseReconnectHandler(uint32_t& sequenceCode, WatchdogConnectionState& watchdogConnectionState);
    ~ResponseReconnectHandler() override = default;

    void handleResponse(std::unique_ptr<Communication::Message<WatchdogModule::Operation>> receivedMessage) override;
};

} // namespace Module