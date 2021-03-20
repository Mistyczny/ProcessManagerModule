#include "ModuleManager.hpp"

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
    if (!instance) {
        throw std::runtime_error("Module server was not initalized");
    }
    return instance->server->sendRequest(identifier, request);
}

} // namespace Module