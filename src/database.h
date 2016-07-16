#ifndef NAIVEDB_DATABASE_H
#define NAIVEDB_DATABASE_H

#include "storage.h"
#include "file.h"
#include "bplus.h"
#include <memory>

namespace naivedb {

class Database {
public:
    Database(const std::string &database);

    ~Database();

    void get(const char *key, void **value, int *len);

    void set(const char *key, const void *value, int len, bool overwrite);

    void remove(const char *key);

private:

    DBStore *db_store_;

    BPlus *tree_;

    const std::string db_name_;

};

}

#endif //NAIVEDB_DATABASE_H
