#pragma once
#include "TimersCache.hpp"

namespace Internal {
[[noreturn]] void TimersThreadMain(TimersCache&);
}