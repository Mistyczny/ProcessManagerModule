#pragma once
#include "ModuleConfiguration.hpp"
#include "Types.hpp"
#include <nlohmann/json.hpp>

namespace Module {

class ConfigurationReader {
private:
    Configuration& configuration;

    std::string configurationDirectoryPath{"/opt/ProcessManager/ModulesConfigurations/"};
    nlohmann::json configJson{};

    bool readConfig();

public:
    explicit ConfigurationReader(Configuration& configuration);

    bool readConfiguration(Types::ModuleIdentifier myModuleIdentifier);
};

} // namespace Module