#include "ModuleConfigurationReader.hpp"
#include "Logging.hpp"
#include <fstream>

namespace Module {

ConfigurationReader::ConfigurationReader(Configuration& configuration) : configuration{configuration} {}

bool ConfigurationReader::readConfiguration(const Types::ModuleIdentifier myModuleIdentifier) {
    bool configRead{false};
    std::string configFile{configurationDirectoryPath + std::to_string(myModuleIdentifier)};
    std::ifstream ifs(configFile);
    if (ifs.is_open()) {
        Log::info("Reading configuration of: " + std::to_string(myModuleIdentifier) + " module");
        try {
            configJson = nlohmann::json::parse(ifs);
            configRead = this->readConfig();
        } catch (nlohmann::json::exception& ex) {
            Log::error("Failed to read configuration from json" + std::string(ex.what()));
        }
    } else {
        Log::error("Failed to open configuration file: " + configFile + " module");
    }
    return configRead;
}

bool ConfigurationReader::readConfig() {
    bool readConfig{true};
    auto& serverConfiguration = configuration.getServerConfiguration();
    if (!serverConfiguration.readFromJson(configJson)) {
        readConfig = false;
    } else {
        serverConfiguration.print();
    }
    return readConfig;
}

} // namespace Module