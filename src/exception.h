#ifndef NAIVEDB_EXCEPTION_H
#define NAIVEDB_EXCEPTION_H

#include <exception>

namespace naivedb {


struct NaivedbException : std::exception {
    const char *what() const noexcept {
        return "Database exception.";
    }
};

struct DuplicateException : NaivedbException {
    const char *what() const noexcept {
        return "Duplicate key.";
    }
};

struct SizeLimitException : NaivedbException {
    const char *what() const noexcept {
        return "Record size exceeds limitation.";
    }
};

struct AllocException : NaivedbException {
    const char *what() const noexcept {
        return "Error allocating space.";
    }
};

struct FileIOException : NaivedbException {
    const char *what() const noexcept {
        return "Error reading or writing files.";
    }
};

struct NotFoundException : NaivedbException {
    const char *what() const noexcept {
        return "Key not found.";
    }
};
}
#endif //NAIVEDB_EXCEPTION_H
