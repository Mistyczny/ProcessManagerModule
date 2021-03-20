#pragma once
#include "ModuleServer.hpp"
#include "Types.hpp"
#include <memory>

namespace Module {

class Manager {
private:
    static inline std::unique_ptr<Manager> instance{nullptr};
    std::shared_ptr<Server> server;

public:
    explicit Manager(std::shared_ptr<Server> server);
    static bool initialize(std::shared_ptr<Server>);

    static bool sendRequest(Types::ServiceIdentifier identifier, google::protobuf::Any* request);
};

} // namespace Module