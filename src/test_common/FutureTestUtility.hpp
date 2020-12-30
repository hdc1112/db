#pragma once

#include "TestParameters.hpp"

#define WAIT_FOR(future) future.wait_for(g_waitForMillis)