#include "ModuleServer.hpp"
#include "ServiceModule.pb.h"
#include "Types.hpp"
#include <catch2/catch.hpp>
#include <gmock/gmock.h>

class MockModuleServer : public Module::Server {
protected:
public:
    MOCK_METHOD2(sendToDestination, bool(const Destination& destination, std::shared_ptr<std::string> message));

    MockModuleServer(boost::asio::io_context& ioContext,
                     std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint>& servicesEndpointsMap,
                     Internal::TimersCache& timersCache)
        : Module::Server(ioContext, servicesEndpointsMap, timersCache) {}

    void sendToDestinationMock() { this->sendToDestination({}, {}); }
};

TEST_CASE("Testing module-service messaging functionality", "[ModuleTests]") {
    REQUIRE(1 == 1);
    boost::asio::io_context ioContext;
    std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint> servicesEndpointsMap;
    Internal::TimersCache timersCache;
    std::shared_ptr<MockModuleServer> mockedModuleServer = std::make_shared<MockModuleServer>(ioContext, servicesEndpointsMap, timersCache);

    ServiceModule::Message message{};
    google::protobuf::Any* anyType = new google::protobuf::Any{};

    for (unsigned short i = 0; i < 10; i++) {
        auto endpoint = boost::asio::ip::udp::endpoint{boost::asio::ip::address::from_string("127.0.0.1"), i};
        servicesEndpointsMap.emplace(Types::toServiceIdentifier(i), endpoint);
    }

    SECTION("Service not found in servicesEndpointsMap") {
        Types::ServiceIdentifier serviceIdentifier{Types::toServiceIdentifier(99)};
        REQUIRE(mockedModuleServer->sendRequest(serviceIdentifier, anyType) == false);
        REQUIRE(timersCache.size() == 0);
    }

    SECTION("Service not found in servicesEndpointsMap") {
        Types::ServiceIdentifier serviceIdentifier{Types::toServiceIdentifier(99)};
        REQUIRE(mockedModuleServer->sendRequest(serviceIdentifier, message) == false);
        REQUIRE(timersCache.size() == 0);
    }

    SECTION("Service found in servicesEndpointsMap") {
        auto endpoint = boost::asio::ip::udp::endpoint{boost::asio::ip::address::from_string("127.0.0.1"), 1};
        Types::ServiceIdentifier serviceIdentifier{Types::toServiceIdentifier(1)};
        Destination expectedDestination{};
        expectedDestination.endpoint = endpoint;
        expectedDestination.serviceIdentifier = serviceIdentifier;
        EXPECT_CALL(*mockedModuleServer, sendToDestination(expectedDestination, testing::_));

        REQUIRE(mockedModuleServer->sendRequest(serviceIdentifier, anyType) == true);
        REQUIRE(timersCache.size() == 1);
    }

    SECTION("Service found in servicesEndpointsMap") {
        auto endpoint = boost::asio::ip::udp::endpoint{boost::asio::ip::address::from_string("127.0.0.1"), 5};
        Types::ServiceIdentifier serviceIdentifier{Types::toServiceIdentifier(5)};
        Destination expectedDestination{};
        expectedDestination.endpoint = endpoint;
        expectedDestination.serviceIdentifier = serviceIdentifier;

        EXPECT_CALL(*mockedModuleServer, sendToDestination(expectedDestination, testing::_));
        REQUIRE(mockedModuleServer->sendRequest(serviceIdentifier, message) == true);
        REQUIRE(timersCache.size() == 1);
    }
}