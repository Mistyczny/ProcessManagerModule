#pragma once
#include "ModuleResponseHandler.hpp"
#include "ModuleWatchdogConnectionState.hpp"
#include "WatchdogModule.pb.h"

namespace Module {

class ResponseConnectHandler : public ModuleResponseHandler {
protected:
    uint32_t& sequenceCode;
    WatchdogConnectionState& watchdogConnectionState;
    WatchdogModule::ConnectResponseData connectResponseData{};

    void handleReceivedMessage();

    [[nodiscard]] ConnectionState onResponseCodeSuccess();

public:
    explicit ResponseConnectHandler(uint32_t& sequenceCode, WatchdogConnectionState& watchdogConnectionState);
    ~ResponseConnectHandler() override = default;
    void handleResponse(std::unique_ptr<Communication::Message<WatchdogModule::Operation>> receivedMessage) override;
};

} // namespace Module