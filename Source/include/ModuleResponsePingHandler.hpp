#pragma once
#include "Communication.hpp"
#include "ModuleResponseHandler.hpp"

namespace Module {

class ResponsePingHandler : public ModuleResponseHandler {
protected:
    uint32_t& sequenceCode;
    std::function<void()> pingTimerSet;

public:
    explicit ResponsePingHandler(uint32_t& sequenceCode, std::function<void()> setTimer);
    ~ResponsePingHandler() override = default;

    void handleResponse(std::unique_ptr<Communication::Message<WatchdogModule::Operation>> receivedMessage) override;
};

} // namespace Module