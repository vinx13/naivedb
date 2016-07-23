#ifndef NAIVEDB_DBSTORE_H
#define NAIVEDB_DBSTORE_H

#include <vector>
#include <string>
#include <memory>
#include "storage.h"


namespace naivedb {


class IndexFileMgr;

class DataFileMgr;


class DBStore {
public:

    DBStore(const std::string &database);

    IndexRecord *indexRecordAt(const Location &location);

    DataRecord *dataRecordAt(const Location &location);

    void saveData(const void *data, int len);

    Location getRoot();

    void setRoot(const Location &location);

    Location getFirstLeaf();

    void setFirstLeaf(const Location &location);

    Location allocIndex();

private:
    std::string database_;
    std::unique_ptr<IndexFileMgr> index_file_;
    std::unique_ptr<DataFileMgr> data_file_;

    std::string getIndexPrefix() const;

    std::string getDataPrefix() const;

    Location allocData(int min_size);

    void collectIndex(const Location &location);

    void collectData(const Location &location);
};

}

#endif //NAIVEDB_DBSTORE_H
