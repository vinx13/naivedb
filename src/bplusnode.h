#ifndef NAIVEDB_BPLUSNODE_H
#define NAIVEDB_BPLUSNODE_H


#include "location.h"
#include "dbstore.h"
#include "bplusdata.h"


namespace naivedb {

class BPlus;

struct BPlusNodeData;
struct IndexRecord;
struct DataRecord;

class DBStore;

class BPlusNode {

    friend class BPlus;

public:

    BPlusNode(const Location &location, DBStore *db_store);

    bool isLeaf() const;

    void asLeaf();

    void asInternal();

    void init();

    bool isFull();

    const char *getKey(int i);

    Location getKeyLoc(int i);

    IndexRecord *getChildRec(int i);

    DataRecord *getValueRec(int i);

    Location getChildLoc(int i) const;

    Location getValueLoc(int i) const;

    int upperBound(const char *key, bool equal);

    int find(const char *key);

    void addKey(int i, const Location &location);

    void addChild(int i, const Location &location);

    void addValue(int i, const Location &location);

    void removeKey(int i);

    void removeChild(int i);

    void removeValue(int i);

    Location getNext() const;

    void setNext(const Location &location);

private:

    BPlusNodeData *data;

    Location location_;

    DBStore *db_store_;

    int linearSearch(int lo, int hi, const char *key);

    int upperBound(const Location &location, bool equal);

    char *copyKey(const Location &location) const;

    void resetData();

    int binarySearch(int lo, int hi, const char *key);

    bool isEmpty();
};


}

#endif //NAIVEDB_BPLUSNODE_H
