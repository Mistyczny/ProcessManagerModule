#pragma once
#include "Communication.hpp"
#include "ModuleConfiguration.hpp"
#include "ModuleConnection.hpp"
#include "ModuleEventsCache.hpp"
#include "ModuleRegistrationState.hpp"
#include "ModuleServer.hpp"
#include "ModuleUserProcess.hpp"
#include "TimersThreadMain.h"
#include "ModuleWatchdogConnectionState.hpp"
#include "TimersCache.hpp"
#include "Types.hpp"
#include <atomic>
#include <boost/asio.hpp>
#include <map>
#include <memory>
#include <thread>
#include <vector>

namespace Module {

class ModuleBase {
private:
    std::map<Types::ServiceIdentifier, boost::asio::ip::udp::endpoint> servicesEndpointsMap;
    boost::asio::io_context ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_worker;
    Internal::TimersCache timersCache{};
    EventsCache eventsCache{};
    std::shared_ptr<Server> server{nullptr};
    WatchdogConnectionState watchdogConnectionState;
    std::shared_ptr<ModuleWatchdogConnection> connection;
    std::thread ioContextWorkThread;
    std::thread watchdogConnectionWatcherThread;
    std::thread timersThread{Internal::TimersThreadMain, std::ref(timersCache)};
    ModuleUserProcess userTask{};

public:
    ModuleBase(int argc, char* argv[]);
    virtual ~ModuleBase();
    bool waitForRegisterToWatchdog();
    bool startWatchdogConnectionTask();
    int startUserTask(int argc, char* argv[]);
    void moduleShutdownHandler();
    bool startModuleServer();
    bool readConfiguration();
    bool configureModule();

    static void SIGINTSignalHandler(int);

    void joinAll();
};

} // namespace Module