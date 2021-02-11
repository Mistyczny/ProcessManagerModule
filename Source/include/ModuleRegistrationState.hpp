#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace Module {

struct ModuleRegistrationState {
    mutable std::mutex stateLock;
    std::condition_variable stateCondition;
    bool state;
};

} // namespace Module