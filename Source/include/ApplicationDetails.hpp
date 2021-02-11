#pragma once
#include <array>
#include <memory>

namespace ApplicationDetails {

constexpr size_t ModuleIdentifierLength = 16;
constexpr size_t MaxExecutableNameLength = 16;

typedef std::array<char, ModuleIdentifierLength> ModuleIdentifier;
typedef std::array<char, MaxExecutableNameLength> ExecutableName;

} // namespace ApplicationDetails