#include "Logging.hpp"
#include "ModuleBase.hpp"
#include "ModuleGlobals.hpp"
#include "Types.hpp"
#include <boost/signals2.hpp>
#include <iostream>

enum ModuleStartupErrors {
    IncorrectNumberOfArguments = -1,
    TooLongModuleIdentifier = -2,
    StartConnectionTaskFailed = -3,
    WaitForRegistrationTimeout = -4
};

void printHelp() {
    std::cout << "Module usage helper" << std::endl;
    std::cout << "Provide module identifier max 20 characters" << std::endl;
}

ModuleStartupErrors startModuleBase(int argc, char* argv[]) {
    // Provided module name is correct, start logger
    Log::initialize(argv[1], Log::LogLevel::TRACE);

    ModuleStartupErrors returnCode{};
    Module::ModuleBase moduleBase{argc, argv};
    if (!moduleBase.startWatchdogConnectionTask()) {
        returnCode = ModuleStartupErrors::StartConnectionTaskFailed;
    } else if (!moduleBase.waitForRegisterToWatchdog()) {
        Log::critical("waitForRegisterToWatchdog timed out");
        returnCode = ModuleStartupErrors::WaitForRegistrationTimeout;
    } else {
        // User program setup and start
        Log::info("User program started");
        auto userReturnCode = moduleBase.startUserTask(argc, argv);
        Log::info("User program return code : " + std::to_string(userReturnCode));
        // User program left gracefully so send shutdown handler to prevent reconnecting
        moduleBase.moduleShutdownHandler();
    }
    return returnCode;
}

int main(int argc, char* argv[]) {
    ModuleStartupErrors returnCode{};
    if (argc != 2) {
        returnCode = ModuleStartupErrors::IncorrectNumberOfArguments;
        printHelp();
    } else if (!Types::canBeModuleIdentifier(atoi(argv[1]))) {
        returnCode = ModuleStartupErrors::TooLongModuleIdentifier;
        printHelp();
    } else {
        Module::Globals::moduleIdentifier = Types::toModuleIdentifier(atoi(argv[1]));
        if (Module::Globals::moduleIdentifier == -1) {
            throw std::runtime_error("BAD Module identifier");
        }

        // Create fork of running module
        switch (fork()) {
        case 0:
            returnCode = startModuleBase(argc, argv);
            break;
        case -1:
            break;
        default:
            std::cout << "Watchdog started" << std::endl;
            break;
        }
    }
    return returnCode;
}