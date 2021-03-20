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
    if(this->server) {
        throw std::runtime_error("Module server was not initalized");
    }
    return this->server->sendRequest(identifier, request);
}

} // namespace Module