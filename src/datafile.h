#ifndef NAIVEDB_DATAFILE_H
#define NAIVEDB_DATAFILE_H

#include "file.h"
#include "storage.h"
#include "exception.h"

namespace naivedb {


const int DefaultDataFileSize = 1024 * 1024 * 32;

class DataFileMgr : public FileMgr {
public:

    DataFileMgr(const std::string &filename);

    DataFileHeader *getDataHeader();

    Location alloc(int min_size);

    DataRecord *recordAt(const Location &location);

    void collect(const Location &location);

private:

    virtual void initFile(int file_no);

    virtual void initHeader();

    void removeEmptyLocation(int bucket);

    int getSuggestedBucketFetch(int min_size) const;

    int allocAt(const Location &location, int size);

    // decide whether block with specific size is large enough for next allocation
    bool isReusableSize(int size) const;

    Location getFromHead(int bucket);

    Location getFreeLocation(int size);

    void addToHead(const Location &loc);

    int getSuggestedBucketPut(int size);
};

}

#endif //NAIVEDB_DATAFILE_H
