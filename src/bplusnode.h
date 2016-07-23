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

    const char * getKey(int i);

    IndexRecord * getChildRec(int i);

    DataRecord * getValueRec(int i);

    Location getChild(int i)const;

    Location getValue(int i)const;

    int upperBound(const char *key);

    int find(const char *key);

    void addKey(int i, const Location &location);

    void addChild(int i, const Location &location);

    void addValue(int i, const Location &location);

    Location getNext() const;

    void setNext(const Location &location);
private:

    BPlusNodeData *data;

    DBStore *db_store_;

    int linearSearch(int lo, int hi, const char *key);
};


}

#endif //NAIVEDB_BPLUSNODE_H
