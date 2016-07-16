#ifndef NAIVEDB_INDEXFILE_H
#define NAIVEDB_INDEXFILE_H

#include <string>

#include "file.h"
#include "location.h"

namespace naivedb {


struct IndexFileHeader;
struct IndexRecord;

class IndexFile : public File {
public:

    IndexFile(const std::string &filename, DBStore *db_store);

    ~IndexFile();

    IndexFileHeader *getHeader();

    void init();

    IndexRecord *recordAt(int offset);

    int alloc();

    void collect(int offset);

    void addToEmptyHeads(const Location &loc);

    // get the index of the list that contains empty blocks that are not less than min_size
    int getBucketIndex(int min_size) const;

    void reserve(int min_size);

    Location getEmptyLocation(int bucket);

    void removeEmptyLocation(int bucket);

    void initHeader();

    void grow();

    void initEmptyRecords(int index);
};

}
#endif //NAIVEDB_INDEXFILE_H
