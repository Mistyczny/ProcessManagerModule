#include "ModuleBase.hpp"
#include "ApplicationDetails.hpp"
#include "EventManager.hpp"
#include "Logging.hpp"
#include "ModuleConfigurationReader.hpp"
#include "ModuleConnection.hpp"
#include "ModuleGlobals.hpp"
#include "ModuleManager.hpp"
#include "MongoDbEnvironment.hpp"
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

namespace Module {

ModuleBase::ModuleBase(int argc, char* argv[])
    : ioContext{}, m_worker{boost::asio::make_work_guard(ioContext)}, connection{std::make_shared<ModuleWatchdogConnection>(
                                                                          ioContext, watchdogConnectionState)},
      ioContextWorkThread{[](boost::asio::io_context& ioContext) { ioContext.run(); }, std::ref(ioContext)},
      watchdogConnectionWatcherThread{WatchdogConnectionWatcher(), this->connection, std::ref(watchdogConnectionState)} {
    server = std::make_shared<Server>(ioContext, servicesEndpointsMap, timersCache, eventsCache);
    Mongo::DbEnvironment::initialize();
    EventManager::initialize(eventsCache);
}

ModuleBase::~ModuleBase() {
    Log::info("Destroying ModuleBase");
    Globals::isModuleRunning = false;
    watchdogConnectionState.setConnectionStateAndNotifyAll(ConnectionState::Shutdown);
    if (watchdogConnectionWatcherThread.joinable()) {
        watchdogConnectionWatcherThread.join();
    }
    this->ioContext.stop();
    if (ioContextWorkThread.joinable()) {
        ioContextWorkThread.join();
    }
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

int ModuleBase::startUserTask(int argc, char* argv[]) { return userTask.main(argc, argv); }

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

bool ModuleBase::startModuleServer() {
    bool serverStarted{};
    try {
        this->server->bindToListeningSocket();
        this->server->startReading();
        Manager::initialize(this->server);
        serverStarted = true;
    } catch (std::exception& ex) {
        Log::error("ModuleBase::startModuleServer caught exception = " + std::string(ex.what()));
    }
    return serverStarted;
}

bool ModuleBase::readConfiguration() {
    ConfigurationReader configurationReader{Configuration::getInstance()};
    return configurationReader.readConfiguration(Globals::moduleIdentifier);
}

bool ModuleBase::configureModule() {
    bool configureModule{true};
    Log::updateConfiguration(Globals::moduleIdentifier);
    return configureModule;
}

void ModuleBase::joinAll() {
    this->ioContextWorkThread.join();
    this->watchdogConnectionWatcherThread.join();
}

} // namespace Module