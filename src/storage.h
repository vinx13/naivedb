#ifndef MEDB_STORAGE_H
#define MEDB_STORAGE_H

#include <string>
#include <vector>

namespace medb {

const int NumBucket = 19;
const int BucketSizes[] = {
    0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000,
    0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000,
    0x400000, 0x800000
};

#pragma pack(1)

struct Location {
    int file_no;
    int offset;

    bool isNull() const { return file_no < 0; }
};

struct IndexFileHeader {
    int file_size;
    int num_data_files;
    Location empty_heads[NumBucket];
    Location tree_root_;
};

struct DataRecord {
    int block_size;
    union {
        int data_size; // for non-empty record
        Location next; // for empty record
    };

    void *getData() {
        return reinterpret_cast<void *>(&data_size + 1);
    }
};

struct IndexRecord {
    int prev, next;
    int data_size;

    void *getData() {
        return reinterpret_cast<void *>(&data_size + 1);
    }
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
    Location allocIndex(int size);

    // remove discontinuous space in index file
    void removeFragments();

    // make sure the size of index file is larger than min_size
    void reserveIndexSpace(int min_size);

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

    DataRecord *dataRecordAt(const Location &loc);

    IndexRecord *indexRecordAt(int offset);

    Location allocDataOn(int bucket, int size);

    // add the block with space given by dataRecordAt(loc)->block_size to head of empty record lists for next allocation
    void collectSpace(const Location &loc);

    IndexFileHeader *getIndexFileHeader();
};


}

#endif //MEDB_STORAGE_H
