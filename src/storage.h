#ifndef MEDB_STORAGE_H
#define MEDB_STORAGE_H

#include <string>
#include <vector>

#include "bplusdata.h"

namespace medb {

const int NumBucket = 19;
const int BucketSizes[] = {
    0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000,
    0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000,
    0x400000, 0x800000
};

#pragma pack(1)

constexpr size_t max(size_t a, size_t b) { return a > b ? a : b; }

struct IndexFileHeader {
    int index_file_size;
    int num_data_files;
    Location empty_heads[NumBucket];
    int tree_root;
    int empty_index_node_ofs;
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
    char data[max(sizeof(InternalNodeData), sizeof(LeafNodeData))];

    void *getData() {
        return reinterpret_cast<void *>(data);
    }
};

#pragma pack()


}

#endif //MEDB_STORAGE_H
