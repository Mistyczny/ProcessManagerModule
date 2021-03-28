#pragma once
#include "Event.hpp"
#include "ModuleEventsCache.hpp"
#include <map>

class EventManager {
private:
    static inline EventManager* eventManager{nullptr};
    Module::EventsCache& eventsCache;

public:
    explicit EventManager(Module::EventsCache& eventsCache);
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;
    virtual ~EventManager();
    static bool initialize(Module::EventsCache& eventsCache) noexcept;

    static EventManager* getEventManager();

    static void registerNewEventHandler(Types::ServiceIdentifier, std::unique_ptr<EventInterface>);
};