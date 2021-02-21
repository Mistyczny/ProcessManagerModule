#include "ModuleServer.hpp"
#include "Logging.hpp"
#include "ModuleGlobals.hpp"
#include "Timer.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <memory>

namespace Module {

Server::Server(boost::asio::io_context& ioContext, std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint>& servicesEndpointsMap,
               Internal::TimersCache& timersCache)
    : ioContext{ioContext}, servicesEndpointsMap{servicesEndpointsMap}, socket{ioContext}, timersCache{timersCache} {}

bool Server::bindToListeningSocket(uint8_t port) {
    bool socketBind{true};
    boost::asio::ip::udp::endpoint myEndpoint{boost::asio::ip::udp::v4(), port};
    try {
        this->socket.open(boost::asio::ip::udp::v4());
        this->socket.bind(myEndpoint);
    } catch (boost::system::error_code& error) {
        socketBind = false;
    }
    return socketBind;
}

uint32_t Server::generateTransactionCode() const {
    uint32_t newTransactionCode = rand() % 1000 + 1;
    auto iter = std::find_if(std::begin(parkedMessages), std::end(parkedMessages),
                             [newTransactionCode](auto& parkedMessage) { return parkedMessage.transactionCode == newTransactionCode; });
    while (iter != std::end(parkedMessages)) {
        newTransactionCode = rand() % 10000000 + 1;
        iter = std::find_if(std::begin(parkedMessages), std::end(parkedMessages),
                            [newTransactionCode](auto& parkedMessage) { return parkedMessage.transactionCode == newTransactionCode; });
    }
    return newTransactionCode;
}

void Server::startReading() {
    this->socket.async_receive_from(
        boost::asio::buffer(messageBuffer), remoteEndpoint,
        boost::bind(&Module::Server::handleReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Server::handleReceive(const boost::system::error_code& error, std::size_t bytesRead) {
    if (!error) {
        this->startReading();
    } else {
        std::string receivedMessage{};
        std::copy_n(messageBuffer.begin(), bytesRead, std::back_inserter(receivedMessage));

        ServiceModule::Message message{};
        message.ParseFromString(receivedMessage);
    }
}

void Server::sendRequest(uint32_t identifier, google::protobuf::Any* anyRequest) {
    auto* requestHeader = new ServiceModule::Header{};
    requestHeader->set_senderidentifier(Module::Globals::moduleIdentifier);
    requestHeader->set_operationcode(ServiceModule::OperationCode::ModuleRequest);
    requestHeader->set_transactioncode(this->generateTransactionCode());

    auto* moduleRequest = new ServiceModule::Request{};
    moduleRequest->set_allocated_request(anyRequest);

    ServiceModule::Message messageToSend{};
    messageToSend.set_allocated_header(requestHeader);
    messageToSend.set_allocated_request(moduleRequest);

    this->sendMessage(identifier, messageToSend);
}

void func(ParkedMessage parked) {}

void Server::sendMessage(uint32_t identifier, ServiceModule::Message& messageToSend) {
    auto endpoint = servicesEndpointsMap.find(identifier);
    if (endpoint != std::end(servicesEndpointsMap)) {
        std::string message{};
        messageToSend.SerializeToString(&message);
        std::pair messaged{endpoint->second, message};
        if (this->messageQueue.push(messaged)) {
            std::shared_ptr<ParkedMessage> parkedMessage{};
            parkedMessage->transactionCode = messageToSend.header().transactioncode();
            parkedMessage->destination = endpoint->second;
            parkedMessage->message = message;
            auto parkkk = std::make_unique<Timer<std::pair<std::shared_ptr<Server>, std::shared_ptr<ParkedMessage>>>>(
                timerExpiration, std::make_pair(shared_from_this(), parkedMessage));
            parkedMessage->timerID = timersCache.registerTimer(std::move(parkkk));
            transactionAndTimerID.emplace(parkedMessage->transactionCode, parkedMessage->timerID);

            if (this->messageQueue.size() > 1) {
                Log::trace("Server::startSending - Sending in progress");
            } else {
                auto& [destination, message] = this->messageQueue.front();
                this->socket.async_send_to(boost::asio::buffer(message), destination,
                                           boost::bind(&Module::Server::handleSend, this, boost::asio::placeholders::error,
                                                       boost::asio::placeholders::bytes_transferred));
            }
        } else {
            Log::error("Server::sendMessage - Failed to push message into messages queue");
        }
    } else {
        Log::error("Server::sendMessage - Not found correct endpoint");
    }
}

void Server::handleSend(const boost::system::error_code& error, std::size_t bytesRead) {
    if (error) {
        Log::error("Server::handleSend error");
    } else {
        // Remove send message
        this->messageQueue.pop();
        if (this->messageQueue.empty()) {
            Log::trace("Server::handleSend - All messages sent, leaving");
        } else {
            auto& [destination, message] = this->messageQueue.front();
            this->socket.async_send_to(boost::asio::buffer(message), destination,
                                       boost::bind(&Module::Server::handleSend, this, boost::asio::placeholders::error,
                                                   boost::asio::placeholders::bytes_transferred));
        }
    }
}

void Server::timerExpiration(std::pair<std::shared_ptr<Server>, std::shared_ptr<ParkedMessage>> serverAndTransactionID) {}

} // namespace Module
