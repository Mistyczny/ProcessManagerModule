#include "EventManager.hpp"
#include "GPS.pb.h"
#include "GpsCoordinatesCache.hpp"
#include "ModuleManager.hpp"
#include "ModuleUserProcess.hpp"
#include <chrono>
#include <iostream>
#include <thread>

class CoordinatesUpdateSubscriptionHandler {
private:
    GpsCoordinatesCache& gpsCoordinatesCache;

public:
    CoordinatesUpdateSubscriptionHandler(GpsCoordinatesCache& gpsCoordinatesCache) : gpsCoordinatesCache{gpsCoordinatesCache} {}
    ~CoordinatesUpdateSubscriptionHandler() {}

    bool validate(const google::protobuf::Any& any) { return any.Is<GPS::SubscribeIdentifierRequest>(); }
    void run(const google::protobuf::Any& any) {
        GPS::SubscribeIdentifierRequest identifierRequest{};
        any.UnpackTo(&identifierRequest);
        auto cords = std::make_pair(identifierRequest.x(), identifierRequest.y());
        this->gpsCoordinatesCache.update(identifierRequest.identifier(), cords);
    }
};

/* Use this to initialize environment before running main loop */
ModuleUserProcess::ModuleUserProcess() = default;

ModuleUserProcess::~ModuleUserProcess() = default;

/* Main of your program */
int ModuleUserProcess::main(int argc, char* argv[]) {
    GpsCoordinatesCache gpsCoordinatesCache{};

    auto coordinatesUpdateSubscriptionHandler = std::make_shared<CoordinatesUpdateSubscriptionHandler>(gpsCoordinatesCache);
    std::unique_ptr<EventInterface> newEvent =
        std::make_unique<MessageReceiveEvent<std::shared_ptr<CoordinatesUpdateSubscriptionHandler>>>(coordinatesUpdateSubscriptionHandler);
    EventManager::registerNewEventHandler(872415233, std::move(newEvent));
    GPS::CoordinatesUpdateRequest coordinatesUpdateRequest{};
    Module::Manager::sendSubscribeRequest(Types::toServiceIdentifier(1), coordinatesUpdateRequest.GetTypeName());

    while (true) {
        std::system("clear");
        gpsCoordinatesCache.print();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}