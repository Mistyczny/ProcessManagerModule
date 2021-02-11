#include "ApplicationDetails.hpp"

namespace ApplicationDetails {

bool isModuleIdentifierType(std::string moduleIdentifier) {
    bool isModuleTyped{false};
    if (moduleIdentifier.size() > ModuleIdentifierLength) {
        isModuleTyped = true;
    }
    return isModuleTyped;
}

} // namespace ApplicationDetails