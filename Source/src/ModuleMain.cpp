#include "Logging.hpp"
#include "ModuleBase.hpp"
#include "Types.hpp"
#include <boost/signals2.hpp>

enum ModuleStartupErrors {
    StartConnectionTaskFailed = -3,
    WaitForRegistrationTimeout = -4,
    StartModuleServerFailed = -4,
};

int main(int argc, char* argv[]) {
    ModuleStartupErrors returnCode{};
    // Provided module name is correct, start logger
    Log::initialize("ProcessManager", Log::LogLevel::TRACE);

    Module::ModuleBase moduleBase{argc, argv};
    if (!moduleBase.startWatchdogConnectionTask()) {
        returnCode = ModuleStartupErrors::StartConnectionTaskFailed;
    } else if (!moduleBase.waitForRegisterToWatchdog()) {
        Log::critical("waitForRegisterToWatchdog timed out");
        returnCode = ModuleStartupErrors::WaitForRegistrationTimeout;
    } else if (!moduleBase.startModuleServer()) {
        Log::critical("Failed to start module server socket");
        returnCode = ModuleStartupErrors::StartModuleServerFailed;
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