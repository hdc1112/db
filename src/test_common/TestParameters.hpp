#pragma once

#include <chrono>

// Used for normal future waiting timeout. Change it to inf for step-by-step debugging
extern std::chrono::milliseconds g_waitForMillis;

// Used for a bit longer future waiting timeout. Change it to inf for step-by-step debugging
extern std::chrono::milliseconds g_waitForLongMillis;