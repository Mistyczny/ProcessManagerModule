#include "ModuleEventsCache.hpp"
#include "ModuleServer.hpp"
#include "MongoDbEnvironment.hpp"
#include "MongoServicesCollection.hpp"
#include "ServiceModule.pb.h"
#include "Types.hpp"
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <catch2/catch.hpp>
#include <gmock/gmock.h>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

class MockModuleServer : public Module::Server {
protected:
public:
    MOCK_METHOD2(sendToDestination, bool(const Destination& destination, std::shared_ptr<std::string> message));

    MockModuleServer(boost::asio::io_context& ioContext,
                     std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint>& servicesEndpointsMap,
                     Internal::TimersCache& timersCache, Module::EventsCache& eventsCache)
        : Module::Server(ioContext, servicesEndpointsMap, timersCache, eventsCache) {}

    void sendToDestinationMock() { this->sendToDestination({}, {}); }
};

class MongoDbConnection {
private:
    std::unique_ptr<Mongo::ServicesCollection> servicesCollection{nullptr};

    void prepare() {
        auto modulesCollectionEntry = Mongo::DbEnvironment::getInstance()->getClient();
        mongocxx::client& client = *modulesCollectionEntry;
        mongocxx::collection servicesCollection{client["ProcessManager"]["Services"]};

        auto builder = document{};
        bsoncxx::document::value newService = builder << "ServiceIdentifier" << Types::toServiceIdentifier(100) // To prevent line move
                                                      << "IpAddress"
                                                      << "127.0.0.1"   // To prevent line move
                                                      << "Port" << 101 // To prevent line move by clang
                                                      << "Name"
                                                      << "ExampleService" // To prevent line move by clang
                                                      << finalize;
        servicesCollection.insert_one(std::move(newService));
    }

public:
    MongoDbConnection() {
        if (!Mongo::DbEnvironment::initialize()) {
            throw std::runtime_error("Failed to initialize connection with mongodb");
        } else if (!Mongo::DbEnvironment::isConnected()) {
            throw std::runtime_error("Not connected with mongodb");
        } else {
            auto modulesCollectionEntry = Mongo::DbEnvironment::getInstance()->getClient();
            servicesCollection = std::make_unique<Mongo::ServicesCollection>(*modulesCollectionEntry, "Services");
            REQUIRE(servicesCollection != nullptr);
            this->prepare();
        }
    }
    ~MongoDbConnection() {
        auto modulesCollectionEntry = Mongo::DbEnvironment::getInstance()->getClient();
        mongocxx::client& client = *modulesCollectionEntry;
        mongocxx::collection servicesCollection{client["ProcessManager"]["Services"]};
        auto builder = document{};
        bsoncxx::document::value entry = builder << "ServiceIdentifier" << Types::toServiceIdentifier(100) << finalize;
        servicesCollection.delete_one(std::move(entry));
    }

    std::unique_ptr<Mongo::ServicesCollection>& getServicesCollection() { return this->servicesCollection; }
};

TEST_CASE_METHOD(MongoDbConnection, "Testing module-service messaging functionality", "[ModuleTests]") {
    boost::asio::io_context ioContext;
    std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint> servicesEndpointsMap;
    Internal::TimersCache timersCache;
    Module::EventsCache eventsCache{};
    std::shared_ptr<MockModuleServer> mockedModuleServer =
        std::make_shared<MockModuleServer>(ioContext, servicesEndpointsMap, timersCache, eventsCache);

    ServiceModule::Message message{};
    auto* anyType = new google::protobuf::Any{};

    for (unsigned short i = 0; i < 10; i++) {
        auto endpoint = boost::asio::ip::udp::endpoint{boost::asio::ip::address::from_string("127.0.0.1"), i};
        servicesEndpointsMap.emplace(Types::toServiceIdentifier(i), endpoint);
    }

    SECTION("Service not found in servicesEndpointsMap and database") {
        Types::ServiceIdentifier serviceIdentifier{Types::toServiceIdentifier(99)};
        REQUIRE(mockedModuleServer->sendRequest(serviceIdentifier, anyType) == false);
        REQUIRE(timersCache.size() == 0);
    }

    SECTION("Service not found in servicesEndpointsMap and database") {
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

    SECTION("Service not found in servicesEndpointsMap and found in database") {
        auto endpoint = boost::asio::ip::udp::endpoint{boost::asio::ip::address::from_string("127.0.0.1"), 101};
        Types::ServiceIdentifier serviceIdentifier{Types::toServiceIdentifier(100)};
        Destination expectedDestination{};
        expectedDestination.endpoint = endpoint;
        expectedDestination.serviceIdentifier = serviceIdentifier;

        EXPECT_CALL(*mockedModuleServer, sendToDestination(expectedDestination, testing::_));
        REQUIRE(mockedModuleServer->sendRequest(serviceIdentifier, anyType) == true);
        REQUIRE(timersCache.size() == 1);
    }

    SECTION("Service not found in servicesEndpointsMap and found in database") {
        auto endpoint = boost::asio::ip::udp::endpoint{boost::asio::ip::address::from_string("127.0.0.1"), 101};
        Types::ServiceIdentifier serviceIdentifier{Types::toServiceIdentifier(100)};
        Destination expectedDestination{};
        expectedDestination.endpoint = endpoint;
        expectedDestination.serviceIdentifier = serviceIdentifier;

        EXPECT_CALL(*mockedModuleServer, sendToDestination(expectedDestination, testing::_));
        REQUIRE(mockedModuleServer->sendRequest(serviceIdentifier, message) == true);
        REQUIRE(timersCache.size() == 1);
    }
}