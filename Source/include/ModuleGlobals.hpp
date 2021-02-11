#pragma once
#include "Types.hpp"
#include <atomic>

namespace Module {
namespace Globals {

extern std::atomic<bool> isModuleRunning;
extern std::atomic<Types::ModuleIdentifier> moduleIdentifier;

} // namespace Globals
} // namespace Module