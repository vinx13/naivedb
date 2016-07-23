#ifndef NAIVEDB_INDEXFILE_H
#define NAIVEDB_INDEXFILE_H

#include <string>

#include "file.h"
#include "location.h"

namespace naivedb {


const int DefaultIndexFileSize = 1024;

struct IndexFileHeader;
struct IndexRecord;

class IndexFileMgr : public FileMgr {
public:

    IndexFileMgr(const std::string &filename);

    IndexRecord *recordAt(const Location &location);

    Location alloc();

    void collect(const Location &location);

    IndexFileHeader *getIndexHeader();



private:
    virtual void initFile(int file_no);;

    virtual void initHeader();

    void addToHead(const Location &location);
};
}
#endif //NAIVEDB_INDEXFILE_H
