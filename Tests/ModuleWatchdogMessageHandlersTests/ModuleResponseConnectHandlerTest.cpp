#include "ModuleResponseConnectHandler.hpp"
#include "Communication.hpp"
#include "ModuleWatchdogConnectionState.hpp"
#include "Types.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Testing module-watchdog connect functionality", "[ModuleTests]") {
    uint32_t sequenceCode{};
    Module::WatchdogConnectionState watchdogConnectionState{};
    Module::ResponseConnectHandler responseConnectHandler{sequenceCode, watchdogConnectionState};

    SECTION("Received success") {
        sequenceCode = 0;

        SECTION("Has sequence code") {
            auto connectMessage = std::make_unique<Communication::Message<WatchdogModule::Operation>>();
            WatchdogModule::ConnectResponseData connectResponseData{};
            connectResponseData.set_responsecode(WatchdogModule::ConnectResponseData::Success);
            connectResponseData.set_sequencecode(3);
            connectResponseData.SerializeToString(&connectMessage->body);
            responseConnectHandler.handleResponse(std::move(connectMessage));
            REQUIRE(watchdogConnectionState.getConnectionState() == Module::ConnectionState::Connected);
            REQUIRE(sequenceCode == 3);
        }
        SECTION("Doesnt have sequence code") {
            auto connectMessage = std::make_unique<Communication::Message<WatchdogModule::Operation>>();
            WatchdogModule::ConnectResponseData connectResponseData{};
            connectResponseData.set_responsecode(WatchdogModule::ConnectResponseData::Success);
            connectResponseData.SerializeToString(&connectMessage->body);
            responseConnectHandler.handleResponse(std::move(connectMessage));
            REQUIRE(connectResponseData.has_sequencecode() == false);
            REQUIRE(watchdogConnectionState.getConnectionState() == Module::ConnectionState::NotConnected);
        }
    }

    SECTION("Received InvalidConnectionState") {
        auto connectMessage = std::make_unique<Communication::Message<WatchdogModule::Operation>>();
        WatchdogModule::ConnectResponseData connectResponseData{};
        connectResponseData.set_responsecode(WatchdogModule::ConnectResponseData::InvalidConnectionState);
        connectResponseData.SerializeToString(&connectMessage->body);
        responseConnectHandler.handleResponse(std::move(connectMessage));
        REQUIRE(watchdogConnectionState.getConnectionState() == Module::ConnectionState::NotConnected);
        REQUIRE(connectResponseData.has_sequencecode() == false);
    }

    SECTION("Received NotModuleIdentifier") {
        auto connectMessage = std::make_unique<Communication::Message<WatchdogModule::Operation>>();
        WatchdogModule::ConnectResponseData connectResponseData{};
        connectResponseData.set_responsecode(WatchdogModule::ConnectResponseData::NotModuleIdentifier);
        connectResponseData.SerializeToString(&connectMessage->body);
        responseConnectHandler.handleResponse(std::move(connectMessage));
        REQUIRE(watchdogConnectionState.getConnectionState() == Module::ConnectionState::NotConnected);
        REQUIRE(connectResponseData.has_sequencecode() == false);
    }

    SECTION("Received ModuleNotExists") {
        auto connectMessage = std::make_unique<Communication::Message<WatchdogModule::Operation>>();
        WatchdogModule::ConnectResponseData connectResponseData{};
        connectResponseData.set_responsecode(WatchdogModule::ConnectResponseData::ModuleNotExists);
        connectResponseData.SerializeToString(&connectMessage->body);
        responseConnectHandler.handleResponse(std::move(connectMessage));
        REQUIRE(watchdogConnectionState.getConnectionState() == Module::ConnectionState::NotConnected);
        REQUIRE(connectResponseData.has_sequencecode() == false);
    }

    SECTION("Received Unknown") {
        auto connectMessage = std::make_unique<Communication::Message<WatchdogModule::Operation>>();
        WatchdogModule::ConnectResponseData connectResponseData{};
        connectResponseData.set_responsecode(WatchdogModule::ConnectResponseData::Unknown);
        connectResponseData.SerializeToString(&connectMessage->body);
        responseConnectHandler.handleResponse(std::move(connectMessage));
        REQUIRE(watchdogConnectionState.getConnectionState() == Module::ConnectionState::NotConnected);
        REQUIRE(connectResponseData.has_sequencecode() == false);
    }
}