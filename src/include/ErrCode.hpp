#pragma once

#include "TypeAlias.hpp"

#include "fmt/format.h"

#include <cassert>

extern thread_local ErrCode errCode;

constexpr short ERR_ILLEGAL_ARG = 1;
constexpr short ERR_FILE_EXISTED = 2;

std::string strErrCode(ErrCode errorCode) noexcept;