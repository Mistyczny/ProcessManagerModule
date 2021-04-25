#include "ModuleManager.hpp"
#include "Logging.hpp"
#include "ServiceModule.pb.h"

namespace Module {

bool Manager::initialize(std::shared_ptr<Server> server) {
    bool isInitialized{true};
    if (!instance) {
        try {
            instance = std::make_unique<Manager>(server);
        } catch (std::exception& ex) {
            isInitialized = false;
        }
    }
    return isInitialized;
}

Manager::Manager(std::shared_ptr<Server> server) : server{server} {}

bool Manager::sendRequest(Types::ServiceIdentifier identifier, google::protobuf::Any* request) {
    bool requestSend{false};
    if (!instance) {
        Log::error("Module::Manager was not initialized");
    } else if (!instance->server) {
        Log::error("Module::Manager server was not initialized");
    } else {
        requestSend = instance->server->sendRequest(identifier, request);
    }
    Log::trace("Sending request to: " + std::to_string(identifier));
    return requestSend;
}

bool Manager::sendSubscribeRequest(Types::ServiceIdentifier identifier, std::string messageType) {
    bool sendSubscribeRequest{false};
    if (!instance) {
        Log::error("Module::Manager was not initialized");
    } else if (!instance->server) {
        Log::error("Module::Manager server was not initialized");
    } else {
        sendSubscribeRequest = instance->server->sendSubscribeRequest(identifier, messageType);
    }
    Log::trace("Sending request to: " + std::to_string(identifier));
    return sendSubscribeRequest;
}

bool Manager::sendRequest(std::string name, google::protobuf::Any* request) {
    bool requestSend{false};
    if (!instance) {
        Log::error("Module::Manager was not initialized");
    } else if (!instance->server) {
        Log::error("Module::Manager server was not initialized");
    } else {
        Log::trace("We should send request to: " + name);
    }
    Log::trace("Sending request to: " + name);
    return requestSend;
}

bool Manager::sendSubscribeRequest(std::string name, std::string messageType) {
    bool sendSubscribeRequest{false};
    if (!instance) {
        Log::error("Module::Manager was not initialized");
    } else if (!instance->server) {
        Log::error("Module::Manager server was not initialized");
    } else {
        Log::trace("We should send request to: " + name);
    }
    Log::trace("Sending request to: " + name);
    return sendSubscribeRequest;
}

} // namespace Module