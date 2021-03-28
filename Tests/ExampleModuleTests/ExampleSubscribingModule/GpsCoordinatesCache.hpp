#pragma once
#include "Types.hpp"
#include <map>
#include <shared_mutex>

class GpsCoordinatesCache {
private:
    std::shared_mutex mutex;
    std::map<Types::ModuleIdentifier, std::pair<int32_t, int32_t>> coordinatesCache;

public:
    void print() {
        std::shared_lock lock{mutex};
        std::cout.width(10);
        std::cout << " identifier"
                  << "x"
                  << "y" << std::endl;
        std::for_each(std::begin(coordinatesCache), std::end(coordinatesCache), [](auto& coordinates) {
            const Types::ModuleIdentifier& identifier = coordinates.first;
            const auto& [x, y] = coordinates.second;
            std::cout << identifier << "   " << x << "       " << y << std::endl;
        });
    }

    void update(Types::ModuleIdentifier identifier, std::pair<int32_t, int32_t> cords) {
        std::shared_lock lock{mutex};
        auto iter = std::find_if(std::begin(coordinatesCache), std::end(coordinatesCache),
                                 [&](auto& coordinates) { return coordinates.first == identifier; });
        if (iter != std::end(coordinatesCache)) {
            iter->second = cords;
        } else {
            coordinatesCache.emplace(identifier, cords);
        }
    }
};