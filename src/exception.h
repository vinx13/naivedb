#ifndef MEDB_EXCEPTION_H
#define MEDB_EXCEPTION_H

#include <exception>

namespace naivedb {

struct NaivedbException : std::exception {
    const char *what() const noexcept {
        return "Database exception.";
    }
};

struct DuplicateException : NaivedbException {
};

struct SizeLimitException : NaivedbException {
};

struct AllocException : NaivedbException {
};
struct SizeLimitExceptino : NaivedbException {
};

}
#endif //MEDB_EXCEPTION_H
