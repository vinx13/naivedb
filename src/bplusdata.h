#ifndef MEDB_BPLUSDATA_H
#define MEDB_BPLUSDATA_H

#include "file.h"

// define data of nodes that should be stored in file

namespace medb{


const int TreeOrder = 4;

#pragma pack(1)

struct InternalNodeData {
    bool is_leaf;
    Location keys[TreeOrder - 1];
    int children_offset[TreeOrder];
};

struct LeafNodeData {
    bool is_leaf;
    int next;
    Location keys[TreeOrder];
    Location values[TreeOrder];
};

#pragma pack()


}
#endif //MEDB_BPLUSDATA_H
