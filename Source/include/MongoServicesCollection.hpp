#pragma once
#include "Types.hpp"
#include <boost/asio.hpp>
#include <mongocxx/client.hpp>
#include <optional>

namespace Mongo {

struct ServiceRecord {
    uint16_t port;
    Types::ServiceIdentifier identifier;
    std::string ipAddress;
    std::string name;
};

class ServicesCollection {
private:
    mongocxx::collection servicesCollection;

    std::optional<ServiceRecord> viewToServiceRecord(bsoncxx::document::view&);

public:
    ServicesCollection(mongocxx::client& client, std::string collectionName);
    virtual ~ServicesCollection() = default;

    std::optional<ServiceRecord> getService(const Types::ServiceIdentifier& identifier);
    std::optional<ServiceRecord> getService(const std::string& name);
};

} // namespace Mongo