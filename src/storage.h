#ifndef MEDB_STORAGE_H
#define MEDB_STORAGE_H

#include <string>
#include <vector>

namespace medb {


#pragma pack(1)

struct Location {
    int file_no;
    int offset;
};

struct IndexFileHeader {
    int file_size;
    int num_data_files;
    Location empty_heads[NumBucket];
    Location tree_root_;
};

struct DataRecord {
    int block_size, data_size;

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

    std::string getDataFileName() const;

    void createDataFile();

    void createIndexFile();
};


}

#endif //MEDB_STORAGE_H
