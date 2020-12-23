#include "ErrCode.hpp"

thread_local ErrCode errCode;

#define ERROR_MESSAGE(errorString) fmt::format("Error code: {} - {}", errorCode, errorString)

std::string strErrCode(ErrCode errorCode) noexcept {
    switch (errorCode) {
        case ERR_ILLEGAL_ARG:
            return ERROR_MESSAGE("Illegal argument error");
        case ERR_FILE_EXISTED:
            return ERROR_MESSAGE("File already existed");
        // TODO: add debug/release assert here
        default:
            assert(false);
    }
}