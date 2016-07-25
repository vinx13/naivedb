#ifndef NAIVEDB_STORAGE_H
#define NAIVEDB_STORAGE_H

#include <string>
#include <vector>

#include "bplusdata.h"
#include "location.h"

namespace naivedb {

const int NumBucket = 19;
const int BucketSizes[] = {
    0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000,
    0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000,
    0x400000, 0x800000
};

#pragma pack(1)

struct FileHeader {
    int num_files;
};
struct IndexFileHeader : FileHeader {
    Location empty_head, tree_root, first_leaf;
};

struct DataFileHeader : FileHeader {
    Location empty_heads[NumBucket];
};

struct DataRecord {
    int block_size; // size of this record (excluding 'block_size') and empty space after this record
    union {
        int data_size; // for non-empty record, size of data, excluding 'block_size' and 'data_size'
        Location next; // for empty record
    };

    void *getData() {
        return reinterpret_cast<void *>(&data_size + 1);
    }
    static const int NonEmptyHeaderSize = sizeof(int) *2; // block_size  + data_size
};

struct IndexRecord {
    union {
        BPlusNodeData data;
        Location next; // for empty record
    };

    BPlusNodeData *getData() {
        return &data;
    }
};

#pragma pack()


}

#endif //NAIVEDB_STORAGE_H
