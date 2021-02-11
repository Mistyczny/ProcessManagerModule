#pragma once
#include "ModuleConnection.hpp"

namespace Module {
namespace Application {

class ModuleConnectionManager {
private:
    static inline std::unique_ptr<ModuleConnectionManager> instance;
    std::shared_ptr<ModuleWatchdogConnection> connection;

public:
    explicit ModuleConnectionManager(std::shared_ptr<ModuleWatchdogConnection> connection);
    ModuleConnectionManager(const ModuleConnectionManager&) = delete;
    ModuleConnectionManager(ModuleConnectionManager&&) = delete;
    virtual ~ModuleConnectionManager() = default;

    static bool initialize(std::shared_ptr<ModuleWatchdogConnection> connection);
};

} // namespace Application
} // namespace Module