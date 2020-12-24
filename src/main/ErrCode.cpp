#include "ErrCode.hpp"
#include "Assert.hpp"

thread_local ErrCode errCode;

#define ERROR_MESSAGE(errorString) fmt::format("Error code: {} - {}", errorCode, errorString)

std::string strErrCode(ErrCode errorCode) noexcept {
    switch (errorCode) {
        case ERR_ILLEGAL_ARG:
            return ERROR_MESSAGE("Illegal argument error");
        case ERR_FILE_EXISTED:
            return ERROR_MESSAGE("File already existed");
        default:
            DEBUG_ABORT("Cannot understand this error code {}", errorCode);
            return ERROR_MESSAGE("Unknown error code");
    }
}