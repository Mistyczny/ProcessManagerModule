#include "TimersCache.hpp"
#include <future>
#include <iostream>
#include <utility>

namespace Internal {

TimersCache::TimersCache() : timersInvokedStatus{false} {}

TimersCache::~TimersCache() {}

uint32_t TimersCache::generateTimerID() const {
    uint32_t timerID{smallestTimerID};
    bool searchingID{true};
    while (searchingID) {
        auto it = std::begin(timers);
        for (; it != std::end(timers); it++) {
            if (it->second->getTimerID() == timerID) {
                break;
            }
        }
        if (it == std::end(timers)) {
            searchingID = false;
        } else {
            timerID++;
        }
    }
    return timerID;
}

uint32_t TimersCache::internalRegisterTimer(std::unique_ptr<TimerInterface> newTimer) {
    std::unique_lock lock{timersLock};
    uint32_t newTimerID = this->generateTimerID();
    if (newTimer) {
        newTimer->setTimerID(newTimerID);
        timers.insert(std::make_pair(newTimer->getExpirationTimestamp(), std::move(newTimer)));
        timersInvokedStatus = true;
    } else {
        newTimerID = 0;
    }

    return newTimerID;
}

// Register timer into cache
uint32_t TimersCache::registerTimer(std::unique_ptr<TimerInterface> newTimer) {
    uint32_t timerID = internalRegisterTimer(std::move(newTimer));
    timersInvoked.notify_one();
    return timerID;
}

// Register timer into cache, but it does not lock acces to it, use it only in places in which we are already locked
// For example in timer handlers
uint32_t TimersCache::registerTimerNoLock(std::unique_ptr<TimerInterface> newTimer) {
    uint32_t newTimerID = this->generateTimerID();
    if (newTimer) {
        newTimer->setTimerID(newTimerID);
        timers.insert(std::make_pair(newTimer->getExpirationTimestamp(), std::move(newTimer)));
    } else {
        newTimerID = 0;
    }
    return newTimerID;
}

bool TimersCache::internalRemoveTimer(uint32_t timerID) {
    std::unique_lock lock{timersLock};
    bool removed{false};
    auto it = std::find_if(std::begin(timers), std::end(timers), [&](const auto& timer) { return timer.second->getTimerID() == timerID; });
    if (it != std::end(timers)) {
        timers.erase(it);
        removed = true;
        timersInvokedStatus = true;
    }
    return removed;
}

void TimersCache::removeTimer(uint32_t timerID) {
    if (this->internalRemoveTimer(timerID)) {
        timersInvoked.notify_one();
    }
}

size_t TimersCache::size() {
    std::unique_lock lock{timersLock};
    return this->timers.size();
}

std::list<TimersCache::TimersMapT::iterator> TimersCache::getExpiredTimers() {
    std::unique_lock lock{timersLock};
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();

    std::list<TimersMapT::iterator> expiredTimers{};
    TimersMapT::iterator iter = std::begin(timers);
    while (iter != std::end(timers)) {
        if (iter->first < now) {
            expiredTimers.push_back(iter);
        }
        iter++;
    }

    return expiredTimers;
}

void TimersCache::triggerExpiredTimers(std::chrono::high_resolution_clock::time_point timePoint) {
    std::unique_lock lock{timersLock};
    std::for_each(std::begin(timers), std::end(timers), [&](const auto& timer) {
        if (timer.second->getActionStatus() == ActionStatus::Waiting) {
            if (timer.second->getExpirationTimestamp() < timePoint) {
                auto future = std::async(std::launch::async, [&]() { timer.second->runCallbackFunction(); });
            }
        }
    });
}

void TimersCache::restartRepeatableTimers() {
    std::unique_lock lock{timersLock};
    std::list<std::unique_ptr<TimerInterface>> toRestart{};
    auto iter = std::begin(timers);
    while (iter != std::end(timers)) {
        if (iter->second->getActionStatus() == ActionStatus::Ended && iter->second->isRepeatable()) {
            toRestart.push_back(std::move(iter->second));
            iter = timers.erase(iter);
        } else {
            iter++;
        }
    }

    if (!toRestart.empty()) {
        std::for_each(std::begin(toRestart), std::end(toRestart), [&](auto& timer) {
            timer->setActionStatus(ActionStatus::Waiting);
            timer->setStartTimestamp();
            timers.insert(std::make_pair(timer->getExpirationTimestamp(), std::move(timer)));
        });
    }
}

void TimersCache::removeEndedTimers() {
    std::unique_lock lock{timersLock};
    for (auto iter = std::begin(timers); iter != std::end(timers);) {
        if (!iter->second->isRepeatable() && iter->second->getActionStatus() == ActionStatus::Ended) {
            iter = this->timers.erase(iter);
        } else {
            iter++;
        }
    }
}

std::optional<std::chrono::high_resolution_clock::time_point> TimersCache::nextExpirationTimestamp() {
    std::unique_lock lock{timersLock};
    std::optional<std::chrono::high_resolution_clock::time_point> nextExpiration{std::nullopt};
    if (!timers.empty()) {
        nextExpiration = this->timers.begin()->first;
    }
    return nextExpiration;
}

void TimersCache::waitForNextExpiration(std::chrono::high_resolution_clock::time_point waitFor) {
    std::unique_lock lock(timersLock);
    timersInvoked.wait_until(lock, waitFor, [&] { return timersInvokedStatus; });
    timersInvokedStatus = false;
}

void TimersCache::waitForInvoke() {
    std::unique_lock lock(timersLock);
    timersInvoked.wait(lock, [&] { return timersInvokedStatus; });
    timersInvokedStatus = false;
}
#include <iostream>
bool TimersCache::restartTimer(uint32_t timerID) {
    bool timerRestarted{false};
    std::unique_lock lock(timersLock);

    auto iter = std::find_if(std::begin(timers), std::end(timers), [&](auto& timer) { return timer.second->getTimerID() == timerID; });
    if (iter != std::end(timers)) {
        auto timer = std::move(iter->second);
        timer->restartTimer();
        timers.erase(iter);
        timers.insert(std::make_pair(timer->getExpirationTimestamp(), std::move(timer)));
        timerRestarted = true;
    }
    return timerRestarted;
}

} // namespace Internal