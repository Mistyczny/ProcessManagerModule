#pragma once
#include "Event.hpp"
#include "Types.hpp"
#include <vector>
#include "ServiceModule.pb.h"

namespace Module {

class EventsCache {
private:
    std::multimap<Types::ServiceIdentifier, std::unique_ptr<EventInterface>> eventsCache;

public:
    EventsCache() = default;
    virtual ~EventsCache() = default;

    void addEvent(Types::ServiceIdentifier, std::unique_ptr<EventInterface>);
    void runRequestsHandlers(Types::ServiceIdentifier, const ServiceModule::Request& request);
};

} // namespace Module