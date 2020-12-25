#pragma once

#include "TypeAlias.hpp"
#include "diskspace/DiskSpaceConstants.hpp"

#include <string>

namespace diskspace {
bool createNewFile(const char* fileName,
                   BlockNum blockNum = defaultBlockNum,
                   BlockBytes blockBytes = defaultBlockBytes);
}