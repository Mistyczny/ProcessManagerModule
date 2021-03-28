#pragma once
#include "GpsCoordinatesCache.hpp"
#include <array>

struct GpsMobilesMap {
private:
    std::array<std::array<int, 100>, 100> gps;

    void clear();
public:

    void printMap(GpsCoordinatesCache&);
};