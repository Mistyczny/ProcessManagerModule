#pragma once
#include "Event.hpp"
#include <vector>

namespace Module {

class ModuleEventsCache {
private:
    std::vector<std::unique_ptr<Event>> events;

public:
    ModuleEventsCache() = default;
    virtual ~ModuleEventsCache() = default;

    bool addEvent(std::unique_ptr<Event> newEvent);
    bool removeEvent(std::string key);
};

} // namespace Module