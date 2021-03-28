#pragma once
#include "Types.hpp"
#include <shared_mutex>
#include <map>

struct GpsCoordinatesCache {
    mutable std::shared_mutex mutex;
    std::map<Types::ModuleIdentifier, std::pair<int32_t, int32_t>> coordinatesCache;
};