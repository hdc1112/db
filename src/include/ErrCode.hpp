#pragma once

#include "TypeAlias.hpp"

#include "fmt/format.h"

#include <cassert>

extern thread_local ErrCode errCode;

constexpr short ERR_NO_ERROR = 0;
constexpr short ERR_ILLEGAL_ARG = 1;
constexpr short ERR_FILE_EXISTED = 2;
constexpr short ERR_UNSUPPORTED_OS = 3;
constexpr short ERR_NO_SUCH_FILE_OR_DIR = 4;
constexpr short ERR_OPERATION_NOT_PERMITTED = 5;
constexpr short ERR_IO = 6;
constexpr short ERR_SERVICE_UNAVAILABLE = 7;
constexpr short ERR_UNCATEGORIZED = 1000;

std::string strErrCode(ErrCode errorCode) noexcept;

ErrCode errnoToErrCode(int err) noexcept;