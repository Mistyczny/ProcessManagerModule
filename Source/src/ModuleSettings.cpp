#include "ModuleSettings.hpp"

namespace Module {

SettingsReader::SettingsReader(Settings& moduleSettings) : settings{moduleSettings} {}

bool SettingsReader::readSettings() {
    bool settingsRead{true};

    return settingsRead;
}

SettingsWriter::SettingsWriter(Settings& moduleSettings) : settings{moduleSettings} {}

} // namespace Module