#pragma once
#include "ApplicationDetails.hpp"
#include "Communication.hpp"
#include "Connection.hpp"
#include "ModuleGlobals.hpp"
#include "ModuleRegistrationState.hpp"
#include "ModuleResponseHandler.hpp"
#include "ModuleWatchdogConnectionState.hpp"
#include "Types.hpp"
#include <chrono>
#include <fstream>

using namespace std::chrono_literals;

namespace Module {

class ModuleWatchdogConnection : public Connection::TcpConnection<WatchdogModule::Operation> {
protected:
    WatchdogConnectionState& watchdogConnectionState;
    uint32_t sequenceCode;

public:
    explicit ModuleWatchdogConnection(boost::asio::io_context& ioContext, WatchdogConnectionState& watchdogConnectionState);
    ~ModuleWatchdogConnection() override;
    void disconnect() override;

    void onTimerExpiration() override;
    void handleReceivedMessage(std::unique_ptr<Communication::Message<WatchdogModule::Operation>> receivedMessage) override;
    void sendConnectRequest();

    std::unique_ptr<ModuleResponseHandler> getResponseHandler(WatchdogModule::Operation& operationCode);
    void sendPing();
    void sendShutdownRequest();
};

class WatchdogConnectionWatcher {
    void connectModuleToWatchdog(std::shared_ptr<ModuleWatchdogConnection> watchdogConnection,
                                 WatchdogConnectionState& watchdogConnectionState) {
        watchdogConnection->sendConnectRequest();
        auto state = watchdogConnectionState.timedWaitForConnection(10s);
        if (state == ConnectionState::Connected) {
            watchdogConnection->setTimerExpiration(1000);
        } else {
            watchdogConnection->closeSocket();
        }
    }

    void reconnectToWatchdog(std::shared_ptr<ModuleWatchdogConnection> watchdogConnection,
                             WatchdogConnectionState& watchdogConnectionState) {
        while (!watchdogConnection->isConnected() && Globals::isModuleRunning) {
            // Create new socket, each socket can be used only once
            watchdogConnection->makeNewSocket();
            boost::asio::ip::tcp::endpoint endpoint{boost::asio::ip::address::from_string("127.0.0.1"), 1234};
            if (watchdogConnection->connect(endpoint)) {
                this->connectModuleToWatchdog(watchdogConnection, watchdogConnectionState);
            } else {
                // Connect failure, cleanup before next retry
                watchdogConnection->closeSocket();
                std::this_thread::sleep_for(1s);
            }
        }
    }

public:
    void operator()(std::shared_ptr<ModuleWatchdogConnection> connection, WatchdogConnectionState& watchdogConnectionState) {
        while (Globals::isModuleRunning) {
            auto newConnectionState = watchdogConnectionState.waitForConnectionStateChange();
            switch (newConnectionState) {
            case ConnectionState::NotConnected:
                this->reconnectToWatchdog(connection, watchdogConnectionState);
                break;
            case ConnectionState::Connected:
                Log::info("Connected");
                // We are already connected, nothing to be done
                break;
            case ConnectionState::Shutdown:
                // Module is about to be shutdown, leave thread
                return;
            }
        }
    }
};

} // namespace Module