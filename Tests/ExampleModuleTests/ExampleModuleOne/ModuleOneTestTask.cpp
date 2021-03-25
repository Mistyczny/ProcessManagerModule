#include "GPS.pb.h"
#include "Logging.hpp"
#include "ModuleManager.hpp"
#include "ModuleUserProcess.hpp"
#include <chrono>
#include <iostream>
#include <thread>

/* Use this to initialize environment before running main loop */
ModuleUserProcess::ModuleUserProcess() = default;

ModuleUserProcess::~ModuleUserProcess() = default;

/* Main of your program */
int ModuleUserProcess::main(int argc, char* argv[]) {
    int newCoordinates = 1;
    while (true) {
        GPS::CoordinatesUpdateRequest coordinatesUpdateRequest{};
        std::cout << "Sending X: " << newCoordinates << std::endl;
        std::cout << "Sending Y: " << newCoordinates << std::endl;
        coordinatesUpdateRequest.set_x(newCoordinates);
        coordinatesUpdateRequest.set_y(newCoordinates);
        auto* anyType = new google::protobuf::Any{};
        anyType->PackFrom(coordinatesUpdateRequest);
        std::cout << "SENDING REQUEST" << std::endl;

        Log::info("ModuleUserProcess before send");
        Module::Manager::sendRequest(Types::toServiceIdentifier(1), anyType);

        Log::info("ModuleUserProcess post send");
        std::cout << "POST SENDING REQUEST" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        newCoordinates++;
    }
    return 0;
}