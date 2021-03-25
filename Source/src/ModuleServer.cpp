/**
 * @file ModuleServer.cpp
 * @brief Module sending messages
 * @author Kacper Waśniowski
 * @copyright Kacper Waśniowski, All Rights Reserved
 * @date 2021
 */
#include "ModuleServer.hpp"
#include "Logging.hpp"
#include "ModuleGlobals.hpp"
#include "MongoDbEnvironment.hpp"
#include "MongoServicesCollection.hpp"
#include "Timer.hpp"
#include <boost/bind.hpp>
#include <chrono>
#include <memory>

namespace Module {

Server::Server(boost::asio::io_context& ioContext, std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint>& servicesEndpointsMap,
               Internal::TimersCache& timersCache)
    : ioContext{ioContext}, servicesEndpointsMap{servicesEndpointsMap}, socket{ioContext}, timersCache{timersCache} {}

bool Server::bindToListeningSocket() {
    bool socketBind{true};
    auto& serverConfiguration = Configuration::getInstance().getServerConfiguration();
    boost::asio::ip::udp::endpoint myEndpoint{boost::asio::ip::udp::v4(), serverConfiguration.listeningPort};
    try {
        this->socket.open(boost::asio::ip::udp::v4());
        this->socket.bind(myEndpoint);
        this->messageBuffer.resize(serverConfiguration.messageBufferSize);
    } catch (boost::system::error_code& error) {
        Log::critical("Server::bindToListeningSocket: " + error.message());
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
        boost::asio::buffer(messageBuffer), remoteEndpoint, [this](const boost::system::error_code& error, std::size_t bytesRead) {
            if (error) {
                this->processHandleReceiveError(error);
            } else {
                // Reserve all required memory to avoid reallocation
                // and get local copy of received message request
                std::string receivedMessage{};
                receivedMessage.reserve(bytesRead);
                std::copy_n(messageBuffer.begin(), bytesRead, std::back_inserter(receivedMessage));

                // Get local copy of message sender endpoint
                boost::asio::ip::udp::endpoint senderEndpoint{remoteEndpoint};

                // Starting listening for new incoming message
                this->startReading();

                // Start processing received message
                ServiceModule::Message serviceModuleMessage{};
                if (serviceModuleMessage.ParseFromString(receivedMessage)) {
                    if (serviceModuleMessage.has_header()) {
                        if (serviceModuleMessage.has_request()) {
                            this->handleRequest(senderEndpoint, serviceModuleMessage);
                        } else if (serviceModuleMessage.has_response()) {
                            this->handleResponse(serviceModuleMessage);
                        } else {
                            Log::error("Server::handleReceive - Received message without request or response data");
                        }
                    } else {
                        Log::error("Server::handleReceive - Received message without header");
                    }
                } else {
                    Log::error("Server::startReading - Failed to parse message - size: " + std::to_string(bytesRead));
                }
            }
        });
}

bool Server::sendRequest(uint32_t identifier, google::protobuf::Any* anyRequest) {
    Log::trace("Server::sendRequest - send enter");
    bool sendRequest{};
    auto endpoint = servicesEndpointsMap.find(identifier);
    if (endpoint != std::end(servicesEndpointsMap)) {
        Log::trace("Server::sendRequest - getting service from local cache");
        Destination destination{};
        destination.endpoint = endpoint->second;
        destination.serviceIdentifier = identifier;
        auto destinationAndRequestPair = std::make_pair(destination, this->makeRequestMessage(anyRequest));
        // Forward message to sending queue
        Log::trace("Server::sendRequest - sending message to: " + destination.endpoint.address().to_string() + "/" +
                   std::to_string(destination.endpoint.port()));
        if (this->messageQueue.push(destinationAndRequestPair)) {
            if (this->messageQueue.size() > 1) {
                Log::trace("Server::startSending - Sending in progress");
            } else {
                this->send();
                sendRequest = true;
            }
        } else {
            Log::error("Server::sendRequest - Failed to push message into messages queue");
        }
    } else {
        Log::trace("Server::sendRequest - getting service from database");
        // Service was not found in the cache, lets get it from database
        auto servicesCollectionEntry = Mongo::DbEnvironment::getInstance()->getClient();
        Mongo::ServicesCollection services{*servicesCollectionEntry, "Services"};
        auto destinationService = services.getService(identifier);
        if (destinationService.has_value()) {
            auto address = boost::asio::ip::address::from_string(destinationService->ipAddress);
            boost::asio::ip::udp::endpoint serviceEndpoint{address, destinationService->port};
            Destination destination{};
            destination.endpoint = serviceEndpoint;
            destination.serviceIdentifier = identifier;

            auto destinationAndRequestPair = std::make_pair(destination, this->makeRequestMessage(anyRequest));
            Log::trace("Server::sendRequest - sending message to: " + destination.endpoint.address().to_string() + "/" +
                       std::to_string(destination.endpoint.port()));
            // Forward message to sending queue
            if (this->messageQueue.push(destinationAndRequestPair)) {
                if (this->messageQueue.size() > 1) {
                    Log::trace("Server::startSending - Sending in progress");
                } else {
                    this->send();
                    sendRequest = true;
                }
            } else {
                Log::error("Server::sendRequest - Failed to push message into messages queue");
            }
        } else {
            Log::trace("Server::sendRequest - service not found in database");
        }
    }
    return sendRequest;
}

bool Server::sendRequest(uint32_t identifier, ServiceModule::Message& request) {
    bool sendRequest{};
    auto endpoint = servicesEndpointsMap.find(identifier);
    if (endpoint != std::end(servicesEndpointsMap)) {
        Destination destination{};
        destination.endpoint = endpoint->second;
        destination.serviceIdentifier = identifier;
        auto destinationAndRequestPair = std::make_pair(destination, request);

        // Forward message to sending queue
        if (this->messageQueue.push(destinationAndRequestPair)) {
            if (this->messageQueue.size() > 1) {
                Log::trace("Server::startSending - Sending in progress");
            } else {
                sendRequest = true;
                this->send();
            }
        } else {
            Log::error("Server::sendRequest - Failed to push message into messages queue");
        }
    } else {
        Log::trace("Server::sendRequest - getting service from database");
        // Service was not found in the cache, lets get it from database
        auto servicesCollectionEntry = Mongo::DbEnvironment::getInstance()->getClient();
        Mongo::ServicesCollection services{*servicesCollectionEntry, "Services"};
        auto destinationService = services.getService(identifier);
        if (destinationService.has_value()) {
            auto address = boost::asio::ip::address::from_string(destinationService->ipAddress);
            boost::asio::ip::udp::endpoint serviceEndpoint{address, destinationService->port};
            Destination destination{};
            destination.endpoint = serviceEndpoint;
            destination.serviceIdentifier = identifier;

            auto destinationAndRequestPair = std::make_pair(destination, request);

            // Forward message to sending queue
            if (this->messageQueue.push(destinationAndRequestPair)) {
                if (this->messageQueue.size() > 1) {
                    Log::trace("Server::startSending - Sending in progress");
                } else {
                    sendRequest = true;
                    this->send();
                }
            } else {
                Log::error("Server::sendRequest - Failed to push message into messages queue");
            }
        }
    }
    return sendRequest;
}

ServiceModule::Message Server::makeRequestMessage(google::protobuf::Any* anyRequest) {
    auto* requestHeader = new ServiceModule::Header{};
    requestHeader->set_senderidentifier(Module::Globals::moduleIdentifier);
    requestHeader->set_operationcode(ServiceModule::OperationCode::ModuleRequest);
    requestHeader->set_transactioncode(this->generateTransactionCode());
    auto* moduleRequest = new ServiceModule::Request{};
    moduleRequest->set_allocated_request(anyRequest);
    ServiceModule::Message messageToSend{};
    messageToSend.set_allocated_header(requestHeader);
    messageToSend.set_allocated_request(moduleRequest);
    std::cout << "HAS HEADER: " << messageToSend.has_header() << std::endl;
    std::cout << "HAS REQUEST: " << messageToSend.has_request() << std::endl;
    return messageToSend;
}

void Server::send() {
    Log::trace("Server::send - sending message");
    // Get message to send
    auto& [destination, message] = this->messageQueue.front();
    // Start timer
    this->startTimerToResend(destination, message);
    // Create message
    auto messageAsString = std::make_shared<std::string>();
    message.SerializeToString(messageAsString.get());
    this->sendToDestination(destination, messageAsString);
}

void Server::startTimerToResend(const Destination& destination, const ServiceModule::Message& request) {
    const auto transactionCode = request.header().transactioncode();
    auto parkedMessage = std::make_shared<ParkedMessage>();
    parkedMessage->destination = destination;
    parkedMessage->message = request;
    parkedMessage->transactionCode = transactionCode;
    auto resendTimer = std::make_unique<Timer<std::shared_ptr<ParkedMessage>>>(
        [this](std::shared_ptr<ParkedMessage> message) {
            if (message) {
                auto iter = transactionAndTimerID.find(message->transactionCode);
                if (iter != std::end(transactionAndTimerID)) {
                    transactionAndTimerID.erase(iter);
                }
                this->sendRequest(message->destination.serviceIdentifier, message->message);
            }
        },
        parkedMessage);
    auto& serverConfiguration = Configuration::getInstance().getServerConfiguration();
    resendTimer->setDuration(std::chrono::seconds(serverConfiguration.timeBetweenMessageResendsInSeconds));
    const auto resendTimerID = timersCache.registerTimer(std::move(resendTimer));
    // Put timer ID into running transactions lists
    this->transactionAndTimerID.emplace(transactionCode, resendTimerID);
}

bool Server::sendToDestination(const Destination& destination, std::shared_ptr<std::string> message) {
    bool sendToDestination{true};
    if (message) {
        this->socket.async_send_to(boost::asio::buffer(*message), destination.endpoint,
                                   [this, message](const boost::system::error_code& error, std::size_t bytesRead) {
                                       if (error) {
                                           this->processSendError(error);
                                       } else {
                                           // Remove send message
                                           this->messageQueue.pop();
                                           if (this->messageQueue.empty()) {
                                               Log::trace("Server::sendToDestination - All messages sent, leaving");
                                           } else {
                                               this->send();
                                           }
                                       }
                                   });
    } else {
        sendToDestination = false;
        Log::error("Server::sendToDestination: Passed nullptr message to send");
    }
    return sendToDestination;
}

void Server::handleRequest(boost::asio::ip::udp::endpoint, ServiceModule::Message& requestMessage) {}

void Server::handleResponse(ServiceModule::Message& responseMessage) {
    if (!responseMessage.has_response()) {
        Log::error("Server::handleResponse message has no response data");
    } else {
        const auto& headerData = responseMessage.header();
        const auto& responseData = responseMessage.response();

        if (responseData.responsecode() == ServiceModule::Response::Success) {
            auto iter = transactionAndTimerID.find(headerData.transactioncode());
            if (iter != std::end(transactionAndTimerID)) {
                timersCache.removeTimer(iter->second);
                transactionAndTimerID.erase(iter);
            } else {
                Log::error("Server::handleResponse - Not found transaction in cache - skipping");
            }
        } else {
            Log::error("Server::handleResponse - Received failure in response message");
        }
    }
}

void Server::processHandleReceiveError(const boost::system::error_code& error) {}
void Server::processSendError(const boost::system::error_code& error) {}

} // namespace Module
