#pragma once
#include "nlohmann/json.hpp"

namespace Module {

class NetworkSettings {
private:
public:
    NetworkSettings();
    ~NetworkSettings();
};

class Settings {
private:
public:
    Settings() = default;
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;
    ~Settings() = default;
};

class SettingsReader {
private:
    Settings& settings;

public:
    explicit SettingsReader(Settings&);
    SettingsReader(const SettingsReader&) = delete;
    SettingsReader(SettingsReader&&) = delete;
    ~SettingsReader() = default;

    bool readSettings();
};

class SettingsWriter {
private:
    Settings& settings;

public:
    explicit SettingsWriter(Settings&);
    SettingsWriter(const SettingsWriter&) = delete;
    SettingsWriter(SettingsWriter&&) = delete;
    ~SettingsWriter() = default;
};

} // namespace Module