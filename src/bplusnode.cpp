#include "bplusnode.h"
#include "datafile.h"


namespace naivedb {


BPlusNode::BPlusNode(const Location &location, DBStore *db_store) : db_store_(db_store) {
    data = db_store_->indexRecordAt(location)->getData();
}

bool BPlusNode::isLeaf() const { return data->is_leaf; }

void BPlusNode::asLeaf() { data->is_leaf = true; }

void BPlusNode::asInternal() { data->is_leaf = false; }

void BPlusNode::init() { data->init(isLeaf()); }

Location BPlusNode::getChild(int i) const {
    assert(!isLeaf());
    assert(i >= 0 && i < TreeOrder);
    return data->Internal.children[i];
}

Location BPlusNode::getValue(int i) const {
    assert(isLeaf());
    assert(i >= 0 && i < TreeOrder - 1);
    return data->Leaf.values[i];
}

DataRecord *BPlusNode::getValueRec(int i) {
    return db_store_->dataRecordAt(getValue(i));
}

IndexRecord *BPlusNode::getChildRec(int i) {
    return db_store_->indexRecordAt(getChild(i));
}

const char *BPlusNode::getKey(int i) {
    assert(i >= 0 && i < TreeOrder - 1);
    Location loc = data->keys[i];
    assert(!loc.isNull());
    return reinterpret_cast<char *>(db_store_->dataRecordAt(loc)->getData());
}

int BPlusNode::find(const char *key) {
    return linearSearch(0, TreeOrder - 1, key); // TODO: binary search would be better
}

int BPlusNode::linearSearch(int lo, int hi, const char *key) {
    while (lo < hi) {
        if(data->keys[lo].isNull()) break;
        const char *k = getKey(lo);
        int cmp =std::strcmp(k, key);
        if (cmp == 0) {
            return lo;
        } else if(cmp > 0) break;
        ++lo;
    }
    return -1;
}

int BPlusNode::upperBound(const char *key) {
    int i = 0;
    while (i < TreeOrder - 1) {
        if (data->keys[i].isNull() || strcmp(key, getKey(i)) < 0) {
            break;
        }
        ++i;
    }
    return i;
}


void BPlusNode::addKey(int i, const Location &location) {
    assert(i >= 0 && i < TreeOrder - 1);
    data->keys[i] = location;
}

void BPlusNode::addChild(int i, const Location &location) {
    assert(!isLeaf());
    assert(i >= 0 && i < TreeOrder);
    data->Internal.children[i] = location;
}

void BPlusNode::addValue(int i, const Location &location) {
    assert(isLeaf());
    assert(i >= 0 && i < TreeOrder - 1);
    data->Leaf.values[i] = location;
}

Location BPlusNode::getNext() const {
    assert(isLeaf());
    return data->Leaf.next;
}

void BPlusNode::setNext(const Location &location) {
    assert(isLeaf());
    data->Leaf.next = location;
}


}