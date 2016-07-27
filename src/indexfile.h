#ifndef NAIVEDB_INDEXFILE_H
#define NAIVEDB_INDEXFILE_H

#include <string>

#include "file.h"
#include "location.h"

namespace naivedb {


const int DefaultIndexFileSize = 1024 * 1024 * 16;

struct IndexFileHeader;
struct IndexRecord;

class IndexFileMgr : public FileMgr {
public:

    IndexFileMgr(const std::string &filename);

    IndexRecord *recordAt(const Location &location);

    Location alloc();

    void collect(const Location &location);

    IndexFileHeader *getIndexHeader();

protected:
    virtual int getHeaderSize() override { return sizeof(IndexFileHeader);}
    virtual int getFileSize() override { return DefaultIndexFileSize; }

private:
    virtual void initFile(int file_no);;

    virtual void initHeader();

    void addToHead(const Location &location);
};
}
#endif //NAIVEDB_INDEXFILE_H
