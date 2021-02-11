#pragma once
#include "Timer.hpp"
#include <condition_variable>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>

namespace Internal {

class TimersCache {
private:
    std::mutex timersLock;
    std::condition_variable timersInvoked;
    bool timersInvokedStatus;
    std::multimap<std::chrono::high_resolution_clock::time_point, std::unique_ptr<TimerInterface>> timers;

    [[nodiscard]] uint32_t generateTimerID() const;

    uint32_t internalRegisterTimer(std::unique_ptr<TimerInterface> newTimer);
    bool internalRemoveTimer(uint32_t timerID);

public:
    typedef std::multimap<std::chrono::high_resolution_clock::time_point, std::unique_ptr<TimerInterface>> TimersMapT;
    static constexpr uint32_t smallestTimerID = 1;

    TimersCache();
    ~TimersCache();

    [[nodiscard]] uint32_t registerTimer(std::unique_ptr<TimerInterface> newTimer);
    [[nodiscard]] uint32_t registerTimerNoLock(std::unique_ptr<TimerInterface> newTimer);
    void removeTimer(uint32_t timerID);
    [[nodiscard]] size_t size();

    void triggerExpiredTimers(std::chrono::high_resolution_clock::time_point timePoint);
    void restartRepeatableTimers();
    void removeEndedTimers();

    std::list<TimersCache::TimersMapT::iterator> getExpiredTimers();

    std::optional<std::chrono::high_resolution_clock::time_point> nextExpirationTimestamp();

    void waitForNextExpiration(std::chrono::high_resolution_clock::time_point);
    void waitForInvoke();

    bool restartTimer(uint32_t timerID);
};

} // namespace Internal