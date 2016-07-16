#ifndef NAIVEDB_BPLUSDATA_H
#define NAIVEDB_BPLUSDATA_H

#include "file.h"

namespace naivedb {

const int TreeOrder = 4;

#pragma pack(1)

struct InternalNodeData {
    bool is_leaf;
    Location keys[TreeOrder - 1];
    int children_offset[TreeOrder];

    void init() {
        is_leaf = false;
        for (int i = 0; i < TreeOrder - 1; i++) {
            keys[i].init();
            children_offset[i] = -1;
        }
        children_offset[TreeOrder - 1] = -1;
    }
};

struct LeafNodeData {
    bool is_leaf;
    int next;
    Location keys[TreeOrder];
    Location values[TreeOrder];

    void init() {
        is_leaf = true;
        for (int i = 0; i < TreeOrder; i++) {
            keys[i].init();
            values[i].init();
        }
    }
};

#pragma pack()


}
#endif // NAIVEDB_BPLUSDATA_H
