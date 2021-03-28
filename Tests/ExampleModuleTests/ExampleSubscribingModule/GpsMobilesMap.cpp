#include "GpsMobilesMap.hpp"

void GpsMobilesMap::clear() {
    std::for_each(std::begin(gps), std::end(gps),
                  [](auto& column) { std::for_each(std::begin(column), std::end(column), [](auto& row) { row = 0; }); });
}

void GpsMobilesMap::printMap(GpsCoordinatesCache& coordinatesCache) {
    this->clear();
    auto& coordinates = coordinatesCache.coordinatesCache;
    std::for_each(std::begin(coordinatesCache.coordinatesCache), std::end(coordinatesCache.coordinatesCache), [&](auto& coordinate) {
        std::pair cords = coordinate.second;
        gps[cords.first][cords.first]++;
    });
}