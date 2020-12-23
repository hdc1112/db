#include <sys/stat.h>

namespace utils {
inline bool exists(const std::string& fileName) {
    struct stat buffer;
    return stat(fileName.c_str(), &buffer) == 0;
}
} // namespace utils