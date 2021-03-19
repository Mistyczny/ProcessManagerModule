#pragma once
#include "MessageQueue.hpp"
#include "ModuleConfiguration.hpp"
#include "ServiceModule.pb.h"
#include "TimersCache.hpp"
#include "Types.hpp"
#include <array>
#include <boost/asio.hpp>
#include <cstdint>
#include <list>
#include <map>
#include <memory>

struct Destination {
    Types::ServiceIdentifier serviceIdentifier;
    boost::asio::ip::udp::endpoint endpoint;
    bool operator==(const Destination& other) const {
        return this->serviceIdentifier == other.serviceIdentifier && this->endpoint == other.endpoint;
    }
};

struct ParkedMessage {
    uint32_t transactionCode;
    Destination destination;
    ServiceModule::Message message;
};

namespace Module {

typedef std::pair<Destination, ServiceModule::Message> MessageType;

class Server : public std::enable_shared_from_this<Server> {
protected:
    boost::asio::io_context& ioContext;
    std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint>& servicesEndpointsMap;
    Internal::TimersCache& timersCache;

    boost::asio::ip::udp::socket socket;
    std::vector<char> messageBuffer{};
    boost::asio::ip::udp::endpoint remoteEndpoint{};
    MessageQueue<MessageType> messageQueue;
    std::list<ParkedMessage> parkedMessages;
    std::map<uint32_t, uint32_t> transactionAndTimerID;

    void send();
    virtual bool sendToDestination(const Destination& destination, std::shared_ptr<std::string> message);

    uint32_t generateTransactionCode() const;

    void handleRequest(boost::asio::ip::udp::endpoint, ServiceModule::Message&);
    void handleResponse(ServiceModule::Message&);

    void processHandleReceiveError(const boost::system::error_code& error);
    void processSendError(const boost::system::error_code& error);

    ServiceModule::Message makeRequestMessage(google::protobuf::Any*);

    void startTimerToResend(const Destination& destination, const ServiceModule::Message& request);

public:
    explicit Server(boost::asio::io_context& ioContext, std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint>&,
                    Internal::TimersCache& timersCache);
    virtual ~Server() = default;

    bool bindToListeningSocket();
    void startReading();

    bool sendRequest(uint32_t identifier, google::protobuf::Any* request);
    bool sendRequest(uint32_t identifier, ServiceModule::Message& request);
};

} // namespace Module
