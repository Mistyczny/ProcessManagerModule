#pragma once
#include "Timer.hpp"
#include "TimersCache.hpp"
#include <memory>

namespace Module {
namespace Application {

class ModuleTimersManager {
private:
    static inline std::unique_ptr<ModuleTimersManager> instance;
    Internal::TimersCache& timersCache;

public:
    explicit ModuleTimersManager(Internal::TimersCache&);
    virtual ~ModuleTimersManager() = default;
    static bool initialize(Internal::TimersCache&);

    static uint32_t registerTimer(std::unique_ptr<TimerInterface>);
    static void removeTimer(uint32_t);
};

} // namespace Application
} // namespace Module