#ifndef NAIVEDB_DBSTORE_H
#define NAIVEDB_DBSTORE_H

#include <vector>
#include <string>
#include "location.h"

namespace naivedb {

class IndexFile;
class DataFile;
struct IndexRecord;
struct DataRecord;

class DBStore {
public:
    static const std::vector<int> BucketSizes;

    DBStore(const std::string &database);

    // allocate a block not less than min_size in data_ file, a new file may be created if no enough space is available
    Location allocData(int min_size);

    // allocate a block with specified size in index file
    int allocIndex();

    // add the block with space given by dataRecordAt(loc)->block_size to head of empty record lists for next allocation
    void collectDataSpace(const Location &loc);

    // add the block in index file to head of empty record lists for next allocation
    void collectIndexSpace(int offset);

    // remove discontinuous space in index file
    void removeFragments();

    // make sure the size of index file is larger than min_size
    void reserveIndexSpace(int min_size);

    DataRecord *dataRecordAt(const Location &loc);

    IndexRecord *indexRecordAt(int offset);

    std::string getDataFileName(int file_no) const;

    std::string getIndexFileName() const;

    IndexFile *getIndexFile();

    DataFile *getDataFile(int file_no);

private:
    std::string database_;

    IndexFile *index_file_;

    std::vector<DataFile *> data_files_;

    Location allocDataOn(int bucket, int size);

    int createDataFile();
};

}

#endif //NAIVEDB_DBSTORE_H
