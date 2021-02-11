#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>

namespace Module {

enum class ConnectionState { NotConnected, Connected, Shutdown };

class WatchdogConnectionState {
private:
    ConnectionState connectionState{ConnectionState::NotConnected};
    mutable std::mutex connectionStateLock;
    std::condition_variable connectionStateCondition;

    void setConnectionState(ConnectionState& connectionState);

public:
    WatchdogConnectionState() = default;
    WatchdogConnectionState(const WatchdogConnectionState&) = delete;
    WatchdogConnectionState(WatchdogConnectionState&&) = delete;
    WatchdogConnectionState& operator=(const WatchdogConnectionState&) = delete;
    WatchdogConnectionState& operator=(WatchdogConnectionState&&) = delete;
    ~WatchdogConnectionState() = default;

    ConnectionState timedWaitForConnection(std::chrono::duration<int> timestampInSeconds);
    ConnectionState waitForConnectionStateChange();
    void setConnectionStateAndNotifyOne(ConnectionState connectionState);
    void setConnectionStateAndNotifyAll(ConnectionState connectionState);

    [[nodiscard]] ConnectionState getConnectionState() const;
};

} // namespace Module