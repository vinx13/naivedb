#ifndef NAIVEDB_BPLUSDATA_H
#define NAIVEDB_BPLUSDATA_H

#include "location.h"

namespace naivedb {

const int TreeOrder = 4;

#pragma pack(1)

struct BPlusNodeData {
    bool is_leaf;
    Location keys[TreeOrder - 1];

    union {
        struct {
            Location children[TreeOrder];
        } Internal;

        struct {
            Location values[TreeOrder - 1];
            Location next;
        } Leaf;
    };

    void init(bool is_leaf = false) {
        this->is_leaf = is_leaf;
        for (auto &loc:keys) {
            loc.init();
        }
        for (auto &loc:Internal.children) {
            loc.init();
        }
    }
};

#pragma pack()


}
#endif // NAIVEDB_BPLUSDATA_H
