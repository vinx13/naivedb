#include <cstring>

#include "bplusnode.h"


namespace naivedb {


BPlusNode::BPlusNode(const Location &location, DBStore *db_store) : db_store_(db_store), location_(location) {
    assert(db_store_);
    resetData();
}

bool BPlusNode::isFull() { return !data->keys[TreeOrder - 1 - 1].isNull(); }

bool BPlusNode::isLeaf() const { return data->is_leaf; }

void BPlusNode::asLeaf() { data->is_leaf = true; }

void BPlusNode::asInternal() { data->is_leaf = false; }

void BPlusNode::init() { data->init(isLeaf()); }

Location BPlusNode::getChildLoc(int i) const {
    assert(!isLeaf());
    assert(i >= 0 && i < TreeOrder);
    return data->Internal.children[i];
}

Location BPlusNode::getValueLoc(int i) const {
    assert(isLeaf());
    assert(i >= 0 && i < TreeOrder - 1);
    return data->Leaf.values[i];
}

DataRecord *BPlusNode::getValueRec(int i) {
    resetData();
    return db_store_->dataRecordAt(getValueLoc(i));
}

IndexRecord *BPlusNode::getChildRec(int i) {
    resetData();
    return db_store_->indexRecordAt(getChildLoc(i));
}

const char *BPlusNode::getKey(int i) {
    Location loc = getKeyLoc(i);
    assert(!loc.isNull());
    resetData();
    return reinterpret_cast<char *>(db_store_->dataRecordAt(
        loc)->getData()); // Be cautious that memory may be deallocated
}

Location BPlusNode::getKeyLoc(int i) {
    assert(i >= 0 && i < TreeOrder - 1);
    return data->keys[i];
}

int BPlusNode::find(const char *key) {
    return linearSearch(0, TreeOrder - 1, key); // TODO: binary search would be better
}

int BPlusNode::linearSearch(int lo, int hi, const char *key) {
    while (lo < hi) {
        if (data->keys[lo].isNull()) break;
        int cmp = std::strcmp(getKey(lo), key);
        if (cmp == 0) {
            return lo;
        } else if (cmp > 0) break;
        ++lo;
    }
    return -1;
}

int BPlusNode::upperBound(const char *key, bool equal) {
    int i = 0;
    while (i < TreeOrder - 1) {
        int cmp = data->keys[i].isNull() ? 1 : std::strcmp(getKey(i), key);
        if ((equal ? cmp > 0 : cmp >= 0)) {
            break;
        }
        ++i;
    }
    return i;
}

int BPlusNode::upperBound(const Location &location, bool equal) {
    assert(!location.isNull());
    char *buf = copyKey(location);
    int result = upperBound(buf, equal);
    delete[] buf;
    return result;
}

char *BPlusNode::copyKey(const Location &location) const {
    assert(!location.isNull());
    DataRecord *record = db_store_->dataRecordAt(location);
    char *buf = new char[record->data_size];
    std::memcpy(reinterpret_cast<void *>(buf), record->getData(), record->data_size);
    return buf;
}

void BPlusNode::addKey(int i, const Location &location) {
    assert(i >= 0 && i < TreeOrder - 1);
    assert(data->keys[i].isNull());
    data->keys[i] = location;
}

void BPlusNode::addChild(int i, const Location &location) {
    assert(!isLeaf());
    assert(i >= 0 && i < TreeOrder);
    assert(data->Internal.children[i].isNull());
    data->Internal.children[i] = location;
}

void BPlusNode::addValue(int i, const Location &location) {
    assert(isLeaf());
    assert(i >= 0 && i < TreeOrder - 1);
    assert(data->Leaf.values[i].isNull());
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

void BPlusNode::removeKey(int i) {
    assert(i >= 0 && i < TreeOrder - 1);
    data->keys[i].init();
}

void BPlusNode::removeChild(int i) {
    assert(i >= 0 && i < TreeOrder);
    assert(!isLeaf());
    data->Internal.children[i].init();
}

void BPlusNode::removeValue(int i) {
    assert(i >= 0 && i < TreeOrder - 1);
    assert(isLeaf());
    data->Leaf.values[i].init();
}

void BPlusNode::resetData() {
    data = db_store_->indexRecordAt(location_)->getData();
}

}