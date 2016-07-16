#ifndef NAIVEDB_FILE_H
#define NAIVEDB_FILE_H

#include <cassert>
#include "mmap.h"

namespace naivedb {


class DBStore;

class File {
public:
    File(const std::string &filename, DBStore *db_store) : file_(filename), db_store_(db_store) { }

protected:

    MemoryMappedFile file_;
    DBStore *db_store_;

    bool isExist();

};


}

#endif //NAIVEDB_FILE_H
