// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

namespace DefaultINI {

const char* glfw_config_file = R"(
[Controls]
pad_start =
pad_select =
pad_home =
pad_dup =
pad_ddown =
pad_dleft =
pad_dright =
pad_a =
pad_b =
pad_x =
pad_y =
pad_r =
pad_l =
pad_sup =
pad_sdown =
pad_sleft =
pad_sright =

[Core]
# The CPU core to be enabled
# 0 (default): Interpreter, 1: Old interpreter (soon to be deprecated)
cpu_core =

# The refresh rate for the GPU
# Defaults to 30
gpu_refresh_rate =

# The applied frameskip amount. Must be a power of two.
# 0 (default): No frameskip, 1: x2 frameskip, 2: x4 frameskip, 3: x8 frameskip, etc.
frame_skip =


[Data Storage]
# Whether to create a virtual SD card.
# 1 (default): Yes, 0: No
use_virtual_sd =


[Miscellaneous]
# A filter which removes logs below a certain logging level.
# Examples: *:Debug Kernel.SVC:Trace Service.*:Critical
log_filter = *:Info


# Path for to a game file to be autoloaded by Citra.
# Leave blank for no autoloading. Overridden by path passed as a console argument.
autoload_game_path =
)";

}
