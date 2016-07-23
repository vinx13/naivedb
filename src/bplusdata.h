#ifndef NAIVEDB_BPLUSDATA_H
#define NAIVEDB_BPLUSDATA_H

#include "location.h"

namespace naivedb {

const int TreeOrder = 4;

#pragma pack(1)

struct BPlusNode {
    bool is_leaf;
    union {
        Location locs[TreeOrder * 2 - 1];
        struct {
            Location keys[TreeOrder - 1];
            Location children[TreeOrder];
        } Internal;

        struct {
            Location keys[TreeOrder - 1];
            Location values[TreeOrder - 1];
            Location next;
        } Leaf;
    };

    void init(bool is_leaf = false) {
        this->is_leaf = is_leaf;
        for (auto &loc:locs)
            loc.init();
    }
};

#pragma pack()


}
#endif // NAIVEDB_BPLUSDATA_H
