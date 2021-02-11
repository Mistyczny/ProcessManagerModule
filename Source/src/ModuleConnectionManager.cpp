#include "ModuleConnectionManager.hpp"

namespace Module {
namespace Application {

ModuleConnectionManager::ModuleConnectionManager(std::shared_ptr<ModuleWatchdogConnection> connection) : connection{connection} {}

bool ModuleConnectionManager::initialize(std::shared_ptr<ModuleWatchdogConnection> connection) {
    bool initialized{false};
    try {
        if (!instance) {
            instance = std::make_unique<ModuleConnectionManager>(connection);
            if (instance) {
                initialized = true;
            }
        }
    } catch (std::exception& ex) {
    }
    return initialized;
}

} // namespace Application
} // namespace Module