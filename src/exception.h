#ifndef MEDB_EXCEPTION_H
#define MEDB_EXCEPTION_H

#include <exception>

namespace medb {

struct MedbException : std::exception {
    const char *what() const {
        return "Database exception.";
    }
};

}

#endif //MEDB_EXCEPTION_H
