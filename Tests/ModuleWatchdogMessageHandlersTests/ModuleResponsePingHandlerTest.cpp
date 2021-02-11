#include "ModuleResponsePingHandler.hpp"
#include "Communication.hpp"
#include "Types.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Testing module-watchdog connect functionality", "[ModuleTests]") {
    uint32_t sequenceCode{3};
    auto startPingTimer = std::bind([&]() { std::cout << "START PING TIMER"; });
    Module::ResponsePingHandler responsePingHandler{sequenceCode, std::move(startPingTimer)};
    WatchdogModule::PingResponseData pingResponseData{};

    SECTION("Sequence code match") {
        pingResponseData.set_sequencecode(3);
        auto connectMessage = std::make_unique<Communication::Message<WatchdogModule::Operation>>();
        pingResponseData.SerializeToString(&connectMessage->body);
        responsePingHandler.handleResponse(std::move(connectMessage));
    }
}