#include <string>
#include <sys/stat.h>

namespace utils {
inline bool exists(const char* fileName) {
    struct stat buffer {};
    return stat(fileName, &buffer) == 0;
}
} // namespace utils