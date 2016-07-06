#ifndef MEDB_EXCEPTION_H
#define MEDB_EXCEPTION_H

#include <exception>

namespace medb {

struct MedbException : std::exception {
    const char *what() const {
        return "Database exception.";
    }
};

struct DuplicateException : std::exception {
};

struct SizeLimitException : std::exception {
};

struct AllocException : std::exception {
};

}

#endif //MEDB_EXCEPTION_H
