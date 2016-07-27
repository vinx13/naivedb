#ifndef NAIVEDB_BPLUS_H
#define NAIVEDB_BPLUS_H

#include <stack>

#include "storage.h"
#include "dbstore.h"
#include "bplusdata.h"
#include "bplusnode.h"

namespace naivedb {


class DBStore;

class BPlusNode;

class BPlus {
public:

    BPlus(DBStore *db_store);

    ~BPlus();

    void remove(const char *key);

    void set(const char *key, const void *value, int len, bool overwrite = false);

    int get(const char *key, void *value);

private:

    Location root_;

    DBStore *db_store_;

    // find the (unique) leaf data in which key may appear, record the path if parents is not null,
    // return value is guaranteed to be a valid location
    Location findLeaf(const char *key, std::stack<Location> *parents);

    Location insertAtLeaf(const Location &leaf_loc, const Location &key_loc, const Location &value_loc, int index);

    void rcopyKV(BPlusNode *src, int isrc, BPlusNode *dest, int idest);

    void insertAtInternal(Location key, Location child, std::stack<Location> *parents);

    // make a new node < root_, key, child > as new root
    void makeNewRoot(const Location &key, const Location &child);

    void rcopyK(BPlusNode *src, int isrc, BPlusNode *dest, int idest);

    void rcopyC(BPlusNode *src, int isrc, BPlusNode *dest, int idest);

    void lcopyKV(BPlusNode *src, int isrc, BPlusNode *dest, int idest);
};


}

#endif //NAIVEDB_BPLUS_H
