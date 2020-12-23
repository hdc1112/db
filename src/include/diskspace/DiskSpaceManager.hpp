#pragma once

#include "TypeAlias.hpp"
#include "diskspace/DiskSpaceConstants.hpp"

#include <string>

namespace diskspace {
bool createNewFile(const std::string& pathToFile,
                   BlockNum blockNum = defaultBlockNum,
                   BlockBytes blockBytes = defaultBlockBytes,
                   bool fillZero = false);
}