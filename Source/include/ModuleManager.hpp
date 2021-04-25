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

    /***
     * @param identifier - identifier of service to which message should be sent
     * @param request - request to send
     * @return Return if message was send successfully
     */
    static bool sendRequest(Types::ServiceIdentifier identifier, google::protobuf::Any* request);
    /***
     * @param name - name of service to which message should be sent
     * @param request - request to send
     * @return Return if message was send successfully
     */
    static bool sendRequest(std::string name, google::protobuf::Any* request);
    /***
     * @param identifier - identifier of service to which message should be sent
     * @param messageType - message type which should be subscribed
     * @return Return if message was send successfully
     */
    static bool sendSubscribeRequest(Types::ServiceIdentifier identifier, std::string messageType);
    /***
     * @param name - name of service to which message should be sent
     * @param messageType - message type which should be subscribed
     * @return Return if message was send successfully
     */
    static bool sendSubscribeRequest(std::string name, std::string messageType);
};

} // namespace Module