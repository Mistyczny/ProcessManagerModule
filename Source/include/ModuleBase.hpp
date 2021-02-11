#pragma once
#include "Communication.hpp"
#include "ModuleConnection.hpp"
#include "ModuleRegistrationState.hpp"
#include "ModuleSettings.hpp"
#include "ModuleUserProcess.hpp"
#include "ModuleWatchdogConnectionState.hpp"
#include "TimersCache.hpp"
#include "Types.hpp"
#include <atomic>
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <vector>

namespace Module {

class ModuleBase {
private:
    boost::asio::io_context ioContext;
    Settings settings;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_worker;
    WatchdogConnectionState watchdogConnectionState;
    std::shared_ptr<ModuleWatchdogConnection> connection;
    std::thread ioContextWorkThread;
    std::thread watchdogConnectionWatcherThread;

public:
    ModuleBase(int argc, char* argv[]);
    virtual ~ModuleBase();
    bool waitForRegisterToWatchdog();
    bool startWatchdogConnectionTask();
    int startUserTask(int argc, char* argv[]);
    void moduleShutdownHandler();

    static void SIGINTSignalHandler(int);
};

} // namespace Module