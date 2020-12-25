#pragma once

#include "TypeAlias.hpp"

#include "fmt/format.h"

#include <cassert>

extern thread_local ErrCode errCode;

constexpr short ERR_NO_ERROR = 0;
constexpr short ERR_ILLEGAL_ARG = 1;
constexpr short ERR_FILE_EXISTED = 2;
constexpr short ERR_UNSUPPORTED_OS = 3;
constexpr short ERR_UNCATEGORIZED = 1000;

std::string strErrCode(ErrCode errorCode) noexcept;