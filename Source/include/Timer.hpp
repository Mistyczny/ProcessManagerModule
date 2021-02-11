#pragma once
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>

// Declaration of class to avoid loop include, and we want to give permission to this class to access timerID
namespace Internal {
class TimersCache;
}

enum ActionStatus { Waiting = 0, Running, Ended };

class TimerInterface {
protected:
    bool active;
    bool repeatable;
    std::atomic<int> actionStatus;
    uint32_t timerID;
    std::chrono::high_resolution_clock::time_point startTimestamp;
    std::chrono::high_resolution_clock::duration duration;

    // Added to enable TimersCache setting TimerID, and hide this from TimerInterface user
    // Because user should not be allowed to set TimerID, since it may lead to have two timers with the same id
    // If user tries to create timer with ID that already exists
    friend class Internal::TimersCache;
    void setTimerID(uint32_t) noexcept;

public:
    explicit TimerInterface();
    virtual ~TimerInterface() = default;

    void setActive(bool) noexcept;
    [[nodiscard]] bool isActive() const;

    void setRepeatable(bool) noexcept;
    [[nodiscard]] bool isRepeatable() const;

    [[nodiscard]] uint32_t getTimerID() const;

    void setStartTimestamp(std::chrono::high_resolution_clock::time_point = std::chrono::high_resolution_clock::now()) noexcept;
    [[nodiscard]] std::chrono::high_resolution_clock::time_point getStartTimestamp() const;

    void setDuration(std::chrono::high_resolution_clock::duration) noexcept;
    [[nodiscard]] std::chrono::high_resolution_clock::duration getDuration() const;

    void setExpirationTimestamp(std::chrono::high_resolution_clock::time_point) noexcept;
    [[nodiscard]] std::chrono::high_resolution_clock::time_point getExpirationTimestamp() const;

    void setActionStatus(ActionStatus);
    [[nodiscard]] int getActionStatus() const;

    void restartTimer();

    virtual void runCallbackFunction() = 0;
    virtual bool isCallbackFunctionSet() = 0;
};

template <typename T, std::enable_if_t<!std::is_pointer_v<T>, bool> = true> class Timer : public TimerInterface {
private:
    T callbackParameters;
    std::function<void(T)> callbackFunction;

public:
    Timer() = default;
    Timer(std::function<void(T)> callbackFunc, T parameters) : TimerInterface{}, callbackParameters{std::move(parameters)} {
        this->callbackFunction = callbackFunc;
    }
    ~Timer() override = default;

    void setCallbackParameters(T callbackParameters) noexcept { this->callbackParameters = callbackParameters; }

    [[nodiscard]] T getCallbackParameters() const { return this->callbackParameters; }

    void setCallbackFunction(std::function<void(T)> callbackFunction) { this->callbackFunction = callbackFunction; }

    std::function<void(T)> getCallbackFunction() { return this->callbackFunction; }

    void runCallbackFunction() override {
        this->actionStatus = ActionStatus::Running;
        this->callbackFunction(callbackParameters);
        this->actionStatus = ActionStatus::Ended;
    }

    bool isCallbackFunctionSet() override { return callbackFunction != nullptr; }
};