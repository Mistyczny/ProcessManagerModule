#include "GPS.pb.h"
#include "Logging.hpp"
#include "ModuleGlobals.hpp"
#include "ModuleManager.hpp"
#include "ModuleUserProcess.hpp"
#include <chrono>
#include <iostream>
#include <thread>

ModuleUserProcess::ModuleUserProcess() = default;

ModuleUserProcess::~ModuleUserProcess() = default;

/* Main of your program */
int ModuleUserProcess::main(int argc, char* argv[]) {
    int newCoordinates = 1;
    while (true) {
        GPS::CoordinatesUpdateRequest coordinatesUpdateRequest{};
        coordinatesUpdateRequest.set_identifier(Module::Globals::moduleIdentifier);
        coordinatesUpdateRequest.set_x(newCoordinates);
        coordinatesUpdateRequest.set_y(newCoordinates);

        auto* anyType = new google::protobuf::Any{};
        anyType->PackFrom(coordinatesUpdateRequest);

        Module::Manager::sendRequest(Types::toServiceIdentifier(1), anyType);

        std::this_thread::sleep_for(std::chrono::seconds(5));
        newCoordinates++;
    }
    return 0;
}