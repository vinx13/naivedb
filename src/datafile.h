#ifndef NAIVEDB_DATAFILE_H
#define NAIVEDB_DATAFILE_H

#include "file.h"
#include "storage.h"

namespace naivedb {

class DataFile : public File {
public:

    DataFile(const std::string &filename, int file_no, DBStore *db_store);

    ~DataFile();

    void init();

    DataRecord *recordAt(int offset);

    int allocAt(int offset, int size);

    void collect(int offset, int size);

    // decide whether block with specific size is large enough for next allocation
    bool isReusableSize(int size) const;

private:
    int file_no_;
};

}

#endif //NAIVEDB_DATAFILE_H
