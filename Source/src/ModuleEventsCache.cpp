#include "ModuleEventsCache.hpp"

namespace Module {

void EventsCache::addEvent(Types::ServiceIdentifier identifier, std::unique_ptr<EventInterface> event) {
    this->eventsCache.emplace(identifier, std::move(event));
}

void EventsCache::runRequestsHandlers(Types::ServiceIdentifier identifier, const ServiceModule::Request& request) {
    auto iter = std::find_if(std::begin(eventsCache), std::end(eventsCache), [&](auto& event) { return event.first == identifier; });
    if (iter != std::end(eventsCache)) {
        auto range = eventsCache.equal_range(identifier);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second->validateMessage(request.request())) {
                it->second->handleReceivedMessage(request.request());
            }
        }
    }
}

} // namespace Module