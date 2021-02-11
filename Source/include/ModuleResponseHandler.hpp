#pragma once
#include "Communication.hpp"
#include <memory>

namespace Module {

class ModuleResponseHandler {
protected:
public:
    virtual ~ModuleResponseHandler() = default;

    virtual void handleResponse(std::unique_ptr<Communication::Message<WatchdogModule::Operation>> receivedMessage) = 0;
};

} // namespace Module