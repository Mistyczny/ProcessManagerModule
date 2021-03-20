/**
 * @file MongoServicesCollection.hpp
 * @brief Services collection in which service data is stored
 * @author Kacper Waśniowski
 * @copyright Kacper Waśniowski, All Rights Reserved
 * @date 2021
 */
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

    /**
     * @param identifier by which service is read
     * @details Get service from database by service
     */
    std::optional<ServiceRecord> getService(const Types::ServiceIdentifier& identifier);
    /**
     * @param name by which service is read
     * @details Get service from database by name
     */
    std::optional<ServiceRecord> getService(const std::string& name);

    /**
     * @details Tests only implemented function, should not be used by external projects
     */
    void drop();
};

} // namespace Mongo