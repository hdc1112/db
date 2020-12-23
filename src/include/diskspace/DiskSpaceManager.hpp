#pragma once

#include "diskspace/DiskSpaceConstants.hpp"
#include "diskspace/DiskSpaceTypes.hpp"

#include <string>

namespace diskspace {
bool createNewFile(const std::string& pathToFile,
                   BlockNum blockNum = defaultBlockNum,
                   BlockBytes blockBytes = defaultBlockBytes,
                   bool fillZero = false);
}