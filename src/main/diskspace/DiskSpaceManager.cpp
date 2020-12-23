#include "diskspace/DiskSpaceManager.hpp"
#include "ErrCode.hpp"
#include "utility/FileUtility.hpp"

namespace diskspace {
bool createNewFile(const std::string& pathToFile, BlockNum blockNum, BlockBytes blockBytes, bool fillZero) {
    if (utils::exists(pathToFile)) {
        errCode = ERR_FILE_EXISTED;
        return false;
    }

    return true;
}
} // namespace diskspace