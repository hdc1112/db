#include "ErrCode.hpp"
#include "Assert.hpp"

#ifdef __APPLE__
#include <sys/errno.h>
#endif

thread_local ErrCode errCode;

#define ERROR_MESSAGE(errorString) fmt::format("Error code: {} - {}", errorCode, errorString)

std::string strErrCode(ErrCode errorCode) noexcept {
    switch (errorCode) {
        case ERR_NO_ERROR:
            DEBUG_ABORT("No error happened, should not call {}", __func__);
            return ERROR_MESSAGE("No error happened");
        case ERR_ILLEGAL_ARG:
            return ERROR_MESSAGE("Illegal argument error");
        case ERR_FILE_EXISTED:
            return ERROR_MESSAGE("File already existed");
        case ERR_UNSUPPORTED_OS:
            return ERROR_MESSAGE("Unsupported OS");
        case ERR_NO_SUCH_FILE_OR_DIR:
            return ERROR_MESSAGE("No such file or directory");
        case ERR_OPERATION_NOT_PERMITTED:
            return ERROR_MESSAGE("Operation not permitted");
        case ERR_IO:
            return ERROR_MESSAGE("Error I/O");
        case ERR_UNCATEGORIZED:
            return ERROR_MESSAGE("Uncategorized error");
        default:
            DEBUG_ABORT("Cannot understand this error code {}", errorCode);
            return ERROR_MESSAGE("Unknown error code");
    }
}

ErrCode errnoToErrCode(int err) noexcept {
#ifdef __APPLE__
    switch (err) {
        case EPERM:
            return ERR_OPERATION_NOT_PERMITTED;
        case ENOENT:
            return ERR_NO_SUCH_FILE_OR_DIR;
        case EIO:
            return ERR_IO;
        default:
            return ERR_UNCATEGORIZED;
    }

#else
    return ERR_UNCATEGORIZED;
#endif
}
