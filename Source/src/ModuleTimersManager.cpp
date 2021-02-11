#include "ModuleTimersManager.hpp"
#include "Timer.hpp"
#include <exception>
#include <iostream>

namespace Module {
namespace Application {

bool ModuleTimersManager::initialize(Internal::TimersCache& timersCache) {
    bool initialized{false};
    if (!instance) {
        std::unique_ptr<ModuleTimersManager> newInstance = std::make_unique<ModuleTimersManager>(timersCache);
        instance = std::move(newInstance);
        initialized = true;
    }
    return initialized;
}

ModuleTimersManager::ModuleTimersManager(Internal::TimersCache& timersCache) : timersCache{timersCache} {}

// Returns timerID which was assigned to registered timer
uint32_t ModuleTimersManager::registerTimer(std::unique_ptr<TimerInterface> newTimer) {
    if (!newTimer->isCallbackFunctionSet()) {
        throw std::invalid_argument("Timer callback function not set");
    }

    return instance->timersCache.registerTimer(std::move(newTimer));
}

void ModuleTimersManager::removeTimer(uint32_t timerID) { instance->timersCache.removeTimer(timerID); }

} // namespace Application
} // namespace Module