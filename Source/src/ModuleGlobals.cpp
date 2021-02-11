#include "ModuleGlobals.hpp"

namespace Module {
namespace Globals {

std::atomic<bool> isModuleRunning = true;
std::atomic<Types::ModuleIdentifier> moduleIdentifier;

} // namespace Globals
} // namespace Module