#include "Timer.hpp"

TimerInterface::TimerInterface()
    : active{true}, repeatable{false}, timerID{}, startTimestamp{std::chrono::high_resolution_clock::now()}, duration{},
      actionStatus{ActionStatus::Waiting} {}

void TimerInterface::setActive(bool active) noexcept { this->active = active; }

bool TimerInterface::isActive() const { return this->active; }

void TimerInterface::setRepeatable(bool repeatable) noexcept { this->repeatable = repeatable; }

bool TimerInterface::isRepeatable() const { return this->repeatable; }

// setTimerID should not be used by user, because it will be override during adding it to the timers cache
void TimerInterface::setTimerID(uint32_t timerID) noexcept { this->timerID = timerID; }

uint32_t TimerInterface::getTimerID() const { return this->timerID; }

void TimerInterface::setStartTimestamp(std::chrono::high_resolution_clock::time_point startTimestamp) noexcept {
    this->startTimestamp = startTimestamp;
}

std::chrono::high_resolution_clock::time_point TimerInterface::getStartTimestamp() const { return this->startTimestamp; }

void TimerInterface::setDuration(std::chrono::high_resolution_clock::duration duration) noexcept { this->duration = duration; }

std::chrono::high_resolution_clock::duration TimerInterface::getDuration() const { return this->duration; }

void TimerInterface::setExpirationTimestamp(std::chrono::high_resolution_clock::time_point expirationTime) noexcept {
    this->startTimestamp = std::chrono::high_resolution_clock::now();
    this->duration = expirationTime - this->startTimestamp;
}

std::chrono::high_resolution_clock::time_point TimerInterface::getExpirationTimestamp() const {
    return this->startTimestamp + this->duration;
}

void TimerInterface::setActionStatus(ActionStatus actionStatus) { this->actionStatus = actionStatus; }

int TimerInterface::getActionStatus() const { return this->actionStatus; }

void TimerInterface::restartTimer() { this->startTimestamp = std::chrono::high_resolution_clock::now(); }