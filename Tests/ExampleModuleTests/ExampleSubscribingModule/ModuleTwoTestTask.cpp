#include "EventManager.hpp"
#include "GPS.pb.h"
#include "GpsMobilesMap.hpp"
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

    bool validate(const google::protobuf::Any& any) {
        std::cout << any.type_url() << std::endl;
        return any.Is<GPS::SubscribeIdentifierRequest>();
    }
    void run(const google::protobuf::Any& any) {
        GPS::SubscribeIdentifierRequest identifierRequest{};
        any.UnpackTo(&identifierRequest);
        std::cout << "Module: " << identifierRequest.identifier() << std::endl;
        std::cout << "UPDATED CORDS TO: " << identifierRequest.x() << ":" << identifierRequest.y() << std::endl;
    }
};

/* Use this to initialize environment before running main loop */
ModuleUserProcess::ModuleUserProcess() = default;

ModuleUserProcess::~ModuleUserProcess() = default;

/* Main of your program */
int ModuleUserProcess::main(int argc, char* argv[]) {
    GpsCoordinatesCache gpsCoordinatesCache{};

    auto coordinatesUpdateSubscriptionHandler = std::make_shared<CoordinatesUpdateSubscriptionHandler>(gpsCoordinatesCache);
    std::cout << "A" << std::endl;
    std::unique_ptr<EventInterface> newEvent =
        std::make_unique<MessageReceiveEvent<std::shared_ptr<CoordinatesUpdateSubscriptionHandler>>>(coordinatesUpdateSubscriptionHandler);
    std::cout << "B" << std::endl;
    EventManager::registerNewEventHandler(872415233, std::move(newEvent));
    std::cout << "C" << std::endl;

    GPS::CoordinatesUpdateRequest coordinatesUpdateRequest{};
    std::cout << coordinatesUpdateRequest.GetTypeName() << std::endl;
    Module::Manager::sendSubscribeRequest(Types::toServiceIdentifier(1), coordinatesUpdateRequest.GetTypeName());

    while (true) {
        std::cout << "IN MAIN" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}