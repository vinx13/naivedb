#ifndef MEDB_FILE_H
#define MEDB_FILE_H

#include "storage.h"

namespace medb{

#pragma pack(1)
struct Location {
    int file_no;
    int offset;

    void init() { file_no = -1; }

    bool isNull() const { return file_no < 0; }
};
#pragma pack()


class FileMgr {
public:
    static const std::vector<int> BucketSizes;

    FileMgr(const std::string &database);

    void *getIndexFile();

    void *getDataFile(int file_no);

    // allocate a block not less than min_size in data file, a new file may be created if no enough space is available
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

private:
    std::string database_;

    void *index_file_;

    std::vector<void *> data_files_;

    std::string getDataFileName(int file_no) const;

    std::string getIndexFileName() const;

    void *openFile(const std::string &filename) const;

    void createDataFile();

    void createIndexFile();

    // decide whether block with specific size is large enough for next allocation
    bool isReusableSize(int size) const;

    // get the index of the list that contains empty blocks that are not less than min_size
    int getBucketIndex(int min_size);

    Location allocDataOn(int bucket, int size);

    IndexFileHeader *getIndexFileHeader();
};


}

#endif //MEDB_FILE_H
