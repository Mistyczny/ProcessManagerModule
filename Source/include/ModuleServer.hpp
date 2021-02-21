#pragma once
#include "MessageQueue.hpp"
#include "ServiceModule.pb.h"
#include "TimersCache.hpp"
#include "Types.hpp"
#include <array>
#include <boost/asio.hpp>
#include <cstdint>
#include <list>
#include <map>
#include <memory>

struct ParkedMessage {
    uint32_t transactionCode;
    uint32_t timerID;
    boost::asio::ip::udp::endpoint destination;
    std::string message;
};

namespace Module {

typedef std::pair<boost::asio::ip::udp::endpoint, std::string> MessageType;

class Server : public std::enable_shared_from_this<Server> {
private:
    boost::asio::io_context& ioContext;
    std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint>& servicesEndpointsMap;
    std::list<ParkedMessage> parkedMessages;
    std::map<uint32_t, uint32_t> transactionAndTimerID;
    Internal::TimersCache& timersCache;
    boost::asio::ip::udp::socket socket;
    std::array<char, 1024> messageBuffer{};
    boost::asio::ip::udp::endpoint remoteEndpoint{};
    MessageQueue<MessageType> messageQueue;

    void handleReceive(const boost::system::error_code& error, std::size_t bytesRead);
    void sendMessage(uint32_t identifier, ServiceModule::Message& moduleRequest);
    void handleSend(const boost::system::error_code& error, std::size_t bytesRead);

    uint32_t generateTransactionCode() const;
    static void timerExpiration(std::pair<std::shared_ptr<Server>, std::shared_ptr<ParkedMessage>> serverAndTransactionID);

public:
    explicit Server(boost::asio::io_context& ioContext, std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint>&,
                    Internal::TimersCache& timersCache);
    virtual ~Server() = default;

    bool bindToListeningSocket(uint8_t port);
    void startReading();

    void sendRequest(uint32_t identifier, google::protobuf::Any* anyRequest);
};

} // namespace Module
