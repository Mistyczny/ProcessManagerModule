#include "EventManager.hpp"

bool EventManager::initialize(Module::EventsCache& eventsCache) noexcept {
    bool eventManagerInitialized{true};
    if (!eventManager) {
        try {
            eventManager = new EventManager(eventsCache);
        } catch (std::bad_alloc& allocationError) {
            eventManagerInitialized = false;
        }
    }
    return eventManagerInitialized;
}

EventManager::EventManager(Module::EventsCache& eventsCache) : eventsCache{eventsCache} {}

EventManager::~EventManager() {
    if (eventManager) {
        delete eventManager;
    }
}
EventManager* EventManager::getEventManager() { return eventManager; }

void EventManager::registerNewEventHandler(Types::ServiceIdentifier identifier, std::unique_ptr<EventInterface> event) {
    if (eventManager) {
        std::cout << "EVENT ADDED" << std::endl;
        eventManager->eventsCache.addEvent(identifier, std::move(event));
    }
}