#pragma once

#include "diskspace/DiskSpaceConstants.hpp"
#include "diskspace/DiskSpaceTypes.hpp"

#include <string>

namespace disk {
bool createNewDatabaseFile(const std::string& pathToFile,
                           BlockNum blockNum = defaultBlockNum,
                           BlockBytes blockBytes = defaultBlockBytes,
                           bool fillZero = false);
}