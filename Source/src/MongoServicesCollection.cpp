/**
 * @file MongoServicesCollection.cpp
 * @brief Services collection in which service data is stored
 * @author Kacper Waśniowski
 * @copyright Kacper Waśniowski, All Rights Reserved
 * @date 2021
 */
#include "MongoServicesCollection.hpp"
#include "Logging.hpp"
#include "MongoDbEnvironment.hpp"
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

namespace Mongo {

ServicesCollection::ServicesCollection(mongocxx::client& client, std::string collectionName)
    : servicesCollection{client["ProcessManager"][collectionName]} {}

std::optional<ServiceRecord> ServicesCollection::viewToServiceRecord(bsoncxx::document::view& view) {
    std::optional<ServiceRecord> serviceRecord{std::nullopt};
    auto serviceIdentifier = view["ServiceIdentifier"];
    auto ipAddress = view["IpAddress"];
    auto port = view["Port"];
    auto name = view["Name"];
    if (serviceIdentifier.type() != bsoncxx::type::k_int32) {
        Log::error("Failed to get module identifier");
    } else if (ipAddress.type() != bsoncxx::type::k_utf8) {
        Log::error("Failed to get ip address");
    } else if (port.type() != bsoncxx::type::k_int32) {
        Log::error("Failed to get port");
    } else if (name.type() != bsoncxx::type::k_utf8) {
        Log::error("Failed to get name");
    } else {
        serviceRecord = std::make_optional<ServiceRecord>();
        serviceRecord->identifier = serviceIdentifier.get_int32();
        serviceRecord->ipAddress = ipAddress.get_utf8().value.to_string();
        serviceRecord->name = name.get_utf8().value.to_string();
        serviceRecord->port = port.get_int32();
    }
    return serviceRecord;
}

std::optional<ServiceRecord> ServicesCollection::getService(const Types::ServiceIdentifier& serviceIdentifier) {
    std::optional<ServiceRecord> serviceRecord{std::nullopt};
    auto builder = document{};
    bsoncxx::document::value entry = builder << "ServiceIdentifier" << serviceIdentifier << finalize;

    auto result = servicesCollection.find_one(std::move(entry));
    if (result) {
        bsoncxx::document::view view{result->view()};
        serviceRecord = this->viewToServiceRecord(view);
    }
    return serviceRecord;
}

std::optional<ServiceRecord> ServicesCollection::getService(const std::string& name) {
    std::optional<ServiceRecord> serviceRecord{std::nullopt};
    auto builder = document{};
    bsoncxx::document::value entry = builder << "Name" << name << finalize;

    auto result = servicesCollection.find_one(std::move(entry));
    if (result) {
        bsoncxx::document::view view{result->view()};
        serviceRecord = this->viewToServiceRecord(view);
    }
    return serviceRecord;
}

void ServicesCollection::drop() { this->servicesCollection.drop(); }

} // namespace Mongo