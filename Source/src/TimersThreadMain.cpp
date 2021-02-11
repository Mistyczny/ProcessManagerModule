#include "TimersThreadMain.h"
#include <chrono>

using namespace std::chrono_literals;

namespace Internal {
[[noreturn]] void TimersThreadMain(TimersCache& timers) {
    while (1) {
        auto nextExpiration = timers.nextExpirationTimestamp();
        if (nextExpiration.has_value()) {
            timers.waitForNextExpiration(*nextExpiration);
        } else {
            timers.waitForInvoke();
        }

        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        timers.triggerExpiredTimers(now);
        timers.restartRepeatableTimers();
        timers.removeEndedTimers();
    }
}
} // namespace Internal
