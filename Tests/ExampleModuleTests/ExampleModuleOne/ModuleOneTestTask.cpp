#include "ModuleGlobals.hpp"
#include "ModuleUserProcess.hpp"
#include <chrono>
#include <iostream>
#include <thread>

/* Use this to initialize environment before running main loop */
ModuleUserProcess::ModuleUserProcess() { Module::Globals::moduleIdentifier = 704643083; }

ModuleUserProcess::~ModuleUserProcess() {}

/* Main of your program */
int ModuleUserProcess::main(int argc, char* argv[]) {
    while (1) {
        std::cout << "MAIN WOKE UP" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}