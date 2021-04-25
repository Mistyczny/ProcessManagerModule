#include "EventManager.hpp"
#include "GPS.pb.h"
#include "GpsCoordinatesCache.hpp"
#include "ModuleManager.hpp"
#include "ModuleUserProcess.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>

class CoordinatesUpdateSubscriptionHandler {
private:
    GpsCoordinatesCache& gpsCoordinatesCache;

public:
    explicit CoordinatesUpdateSubscriptionHandler(GpsCoordinatesCache& gpsCoordinatesCache) : gpsCoordinatesCache{gpsCoordinatesCache} {}
    ~CoordinatesUpdateSubscriptionHandler() = default;

    bool validate(const google::protobuf::Any& any) { return any.Is<GPS::SubscribeIdentifierRequest>(); }
    void run(const google::protobuf::Any& any) {
        GPS::SubscribeIdentifierRequest identifierRequest{};
        any.UnpackTo(&identifierRequest);
        auto cords = std::make_pair(identifierRequest.x(), identifierRequest.y());
        this->gpsCoordinatesCache.update(identifierRequest.identifier(), cords);
    }
};

ModuleUserProcess::ModuleUserProcess() = default;

ModuleUserProcess::~ModuleUserProcess() = default;

/* Main of your program */
int ModuleUserProcess::main(int argc, char* argv[]) {
    GpsCoordinatesCache gpsCoordinatesCache{};

    auto coordinatesUpdateSubscriptionHandler = std::make_shared<CoordinatesUpdateSubscriptionHandler>(gpsCoordinatesCache);
    std::unique_ptr<EventInterface> newEvent =
        std::make_unique<MessageReceiveEvent<std::shared_ptr<CoordinatesUpdateSubscriptionHandler>>>(coordinatesUpdateSubscriptionHandler);
    EventManager::registerNewEventHandler(Types::toServiceIdentifier(1), std::move(newEvent));

    GPS::CoordinatesUpdateRequest coordinatesUpdateRequest{};
    Module::Manager::sendSubscribeRequest(Types::toServiceIdentifier(1), coordinatesUpdateRequest.GetTypeName());

    while (true) {
        std::system("clear");
        std::cout.width(30);
        std::cout << "GPS Command center" << std::endl;
        std::cout.width(30);
        std::cout << "------------------------------" << std::endl;
        gpsCoordinatesCache.print();
        std::cout.width(30);
        std::cout << "------------------------------" << std::endl;
    }
    return 0;
}