// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <string>

// These are settings that apply only to the Qt frontend.
// Hence, they shouldn't be put in Core but instead here.
namespace Settings {

struct QtValues {
    // Whether a loaded game is automatically started on load.
    bool autoplay_game;

    // A map file path to automatically load on init.
    std::string autoload_map_path;
} extern qt_values;
    
}
