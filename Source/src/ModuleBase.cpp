#include "ModuleBase.hpp"
#include "ApplicationDetails.hpp"
#include "Logging.hpp"
#include "ModuleConnection.hpp"
#include "ModuleGlobals.hpp"
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

namespace Module {

ModuleBase::ModuleBase(int argc, char* argv[])
    : ioContext{}, m_worker{boost::asio::make_work_guard(ioContext)}, connection{std::make_shared<ModuleWatchdogConnection>(
                                                                          ioContext, watchdogConnectionState)},
      ioContextWorkThread{[](boost::asio::io_context& ioContext) { ioContext.run(); }, std::ref(ioContext)},
      watchdogConnectionWatcherThread{WatchdogConnectionWatcher(), this->connection, std::ref(watchdogConnectionState)} {}

ModuleBase::~ModuleBase() {
    Log::info("Destroying ModuleBase");
    watchdogConnectionWatcherThread.join();
    ioContextWorkThread.join();
}

bool ModuleBase::startWatchdogConnectionTask() {
    bool connectionToWatchdogEstablished{true};
    boost::asio::ip::tcp::endpoint endpoint{boost::asio::ip::address::from_string("127.0.0.1"), 1234};
    if (this->connection->connect(endpoint)) {
        Log::info("ModuleBase::startWatchdogConnectionTask connection established");
        connection->sendConnectRequest();
        connection->startReadingSequence();
    } else {
        Log::error("ModuleBase::startWatchdogConnectionTask connection failed for module");
        connectionToWatchdogEstablished = false;
    }
    return connectionToWatchdogEstablished;
}

bool ModuleBase::waitForRegisterToWatchdog() {
    bool connected{false};
    auto connectionState = watchdogConnectionState.timedWaitForConnection(10s);
    if (connectionState == ConnectionState::Connected) {
        this->connection->setTimerExpiration(1000);
        connected = true;
    }
    return connected;
}

int ModuleBase::startUserTask(int argc, char* argv[]) {
    ModuleUserProcess userTask{};
    return userTask.main(argc, argv);
}

void ModuleBase::moduleShutdownHandler() {
    // Set module to stop running
    Globals::isModuleRunning = false;
    Log::info("Sending shutdown request");
    this->connection->sendShutdownRequest();
    // Run all ready handlers, to ensure that shutdown request has been sent
    this->ioContext.poll();
    // Stop io_context so it work thread can leave gracefully
    this->ioContext.stop();
    // Notify all that we are leaving execution
    this->watchdogConnectionState.setConnectionStateAndNotifyAll(ConnectionState::Shutdown);
}

void ModuleBase::SIGINTSignalHandler(int signalNumber) { exit(signalNumber); }

} // namespace Module