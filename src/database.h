#ifndef MEDB_DATABASE_H
#define MEDB_DATABASE_H

#include "storage.h"
#include "file.h"
#include "bplus.h"
#include <memory>

namespace medb {

class Database {
public:
    Database(const std::string &database);

    ~Database();

    void get(const char *key, void **value, int *len);

    void set(const char *key, const void *value, int len);

    void remove(const char *key);

private:

    DBStore *db_store_;

    BPlus *tree_;

    const std::string db_name_;

};

}

#endif //MEDB_DATABASE_H
