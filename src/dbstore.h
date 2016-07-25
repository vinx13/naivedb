#ifndef NAIVEDB_DBSTORE_H
#define NAIVEDB_DBSTORE_H

#include <vector>
#include <string>
#include <memory>

#include "storage.h"
#include "indexfile.h"
#include "datafile.h"


namespace naivedb {


class DBStore {
public:

    DBStore(const std::string &database);

    IndexRecord *indexRecordAt(const Location &location);

    DataRecord *dataRecordAt(const Location &location);

    Location saveData(const void *data, int len);

    void removeData(const Location &location);

    Location getRoot();

    void setRoot(const Location &location);

    Location getFirstLeaf();

    void setFirstLeaf(const Location &location);

    Location allocIndex();

    Location allocData(int min_size);

    void collectIndex(const Location &location);

    void collectData(const Location &location);

private:
    std::string database_;
    std::unique_ptr<IndexFileMgr> index_file_;
    std::unique_ptr<DataFileMgr> data_file_;

    std::string getIndexPrefix() const;

    std::string getDataPrefix() const;

};


}

#endif //NAIVEDB_DBSTORE_H
