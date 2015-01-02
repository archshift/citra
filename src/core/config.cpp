// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <GLFW/glfw3.h>

#include "common/file_util.h"
#include "core/settings.h"
#include "core/config.h"
#include "core/core.h"

namespace Core {

static const char* default_config_file = R"(
[Core]
cpu_core =           ;; 0: Interpreter (default), 1: OldInterpreter (may work better, soon to be deprecated)
gpu_refresh_rate =   ;; 30 (default)
frame_skip =         ;; 0: No frameskip (default), 1 : 2x frameskip, 2 : 4x frameskip, etc.

[Data Storage]
use_virtual_sd =

[Miscellaneous]
log_filter = *:Info  ;; Examples: *:Debug Kernel.SVC:Trace Service.*:Critical
)";

Config::Config() {
    // TODO: Don't hardcode the path; let the frontend decide where to put the config files.
    core_config_loc = FileUtil::GetUserPath(D_CONFIG_IDX) + "core-config.ini";
    core_config = new INIReader(core_config_loc);

    Reload();
}

bool Config::LoadINI(INIReader* config, const char* location, const std::string& default_contents, bool retry) {
    if (config->ParseError() < 0) {
        if (retry) {
            LOG_WARNING(Config, "Failed to load %s. Creating file from defaults...", location);
            FileUtil::CreateFullPath(location);
            FileUtil::WriteStringToFile(true, default_contents, location);
            *config = INIReader(location); // Reopen file

            return LoadINI(config, location, default_contents, false);
        }
        LOG_ERROR(Config, "Failed.");
        return false;
    }
    LOG_INFO(Config, "Successfully loaded %s", location);
    return true;
}

void Config::ReadValues() {
    // Core
    Settings::values.cpu_core = core_config->GetInteger("Core", "cpu_core", Core::CPU_Interpreter);
    Settings::values.gpu_refresh_rate = core_config->GetInteger("Core", "gpu_refresh_rate", 30);
    Settings::values.frame_skip = core_config->GetInteger("Core", "frame_skip", 0);

    // Data Storage
    Settings::values.use_virtual_sd = core_config->GetBoolean("Data Storage", "use_virtual_sd", true);

    // Miscellaneous
    Settings::values.log_filter = core_config->Get("Miscellaneous", "log_filter", "*:Info");
}

void Config::Reload() {
    LoadINI(core_config, core_config_loc.c_str(), default_config_file);
    ReadValues();
}

Config::~Config() {
    delete core_config;
}

}
