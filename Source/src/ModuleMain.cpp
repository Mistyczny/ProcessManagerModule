#include "Logging.hpp"
#include "ModuleBase.hpp"
#include "ModuleGlobals.hpp"
#include "Types.hpp"
#include <boost/program_options.hpp>

enum ModuleStartupErrors {
    FailedToReadConfiguration = 1,
    FailedToConfigureModule,
    StartConnectionTaskFailed,
    WaitForRegistrationTimeout,
    StartModuleServerFailed
};

void validateOptions(int argc, char* argv[]) {
    try {
        boost::program_options::options_description optionsDescription("Allowed options");
        optionsDescription.add_options()("help", "Produce help message")("identifier", boost::program_options::value<uint32_t>(),
                                                                         "set identifier");
        boost::program_options::variables_map variablesMap{};
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, optionsDescription), variablesMap);
        boost::program_options::notify(variablesMap);
        if (variablesMap.contains("identifier")) {
            Module::Globals::moduleIdentifier = variablesMap["identifier"].as<uint32_t>();
        }
    } catch (boost::program_options::error& ex) {
    }
}

int main(int argc, char* argv[]) {
    ModuleStartupErrors returnCode{};
    // Provided module name is correct, start logger
    Log::initialize("ProcessManager", Log::LogLevel::TRACE);
    validateOptions(argc, argv);
    Module::ModuleBase moduleBase{argc, argv};
    if (!moduleBase.readConfiguration()) {
        Log::critical("Failed to read configuration");
        returnCode = ModuleStartupErrors::FailedToReadConfiguration;
    } else if (!moduleBase.configureModule()) {
        Log::critical("Failed to configure module");
        returnCode = ModuleStartupErrors::FailedToConfigureModule;
    } else if (!moduleBase.startWatchdogConnectionTask()) {
        Log::critical("Failed to start watchdog connection task");
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