#include <cstring>

#include "bplus.h"

namespace naivedb {


BPlus::BPlus(DBStore *db_store) : db_store_(db_store) {
    root_ = db_store_->getRoot();

    if (root_.isNull()) {
        root_ = db_store_->allocIndex();
        BPlusNode root_node(root_, db_store_);
        root_node.asLeaf();
        root_node.init();
        db_store_->setRoot(root_);
    }
}

BPlus::~BPlus() {
}


int BPlus::get(const char *key, void *value) {
    Location location = findLeaf(key, nullptr);
    BPlusNode leaf(location, db_store_);
    assert(leaf.isLeaf());
    int index = leaf.find(key);
    if (index < 0) {
        throw NotFoundException();
    }
    DataRecord *record = leaf.getValueRec(index);
    int len = record->data_size;
    std::memcpy(value, record->getData(), len);
    return len;
}


Location BPlus::findLeaf(const char *key, std::stack<Location> *parents) {
    Location location = root_;
    while (true) {
        assert(!location.isNull());
        BPlusNode node(location, db_store_);
        if (node.isLeaf()) {
            return location;
        } else {
            if (parents) parents->push(location);
        }
        int i = node.upperBound(key, true);
        location = node.getChildLoc(i);
    }
}


void BPlus::set(const char *key, const void *value, int len, bool overwrite) {
    std::stack<Location> parents;
    Location leaf_loc = findLeaf(key, &parents);
    BPlusNode leaf(leaf_loc,db_store_);
    assert(leaf.isLeaf());
    int index = leaf.upperBound(key, false);
    bool dup = false;
    if (index < TreeOrder - 1
        && !leaf.getKeyLoc(index).isNull()
        && 0 == std::strcmp(leaf.getKey(index), key)) {
        dup = true;
    }
    if (!overwrite && dup) {
        throw DuplicateException();
    }
    if (dup) {
        // overwrite old record
        // we can simply replace the new location and make no other changes
        db_store_->removeData(leaf.getValueLoc(index));
        leaf.removeValue(index);
        leaf.addValue(index, db_store_->saveData(value, len));
        return;
    }
    Location key_loc = db_store_->saveData(reinterpret_cast<const void *>(key), std::strlen(key) + 1),
        value_loc = db_store_->saveData(value, len);

    Location new_node_loc = insertAtLeaf(leaf_loc, key_loc, value_loc, index);
    if (!new_node_loc.isNull()) {
        insertAtInternal(BPlusNode(new_node_loc, db_store_).getKeyLoc(0), new_node_loc, &parents);
    }
}


Location BPlus::insertAtLeaf(const Location &leaf_loc, const Location &key_loc, const Location &value_loc, int index) {
    BPlusNode leaf(leaf_loc, db_store_);
    if (!leaf.isFull()) {
        assert(index >= 0);
        assert(index < TreeOrder - 1);
        rcopyKV(&leaf, index, &leaf, index + 1);
        leaf.addKey(index, key_loc);
        leaf.addValue(index, value_loc);
        return Location::None;
    }

    Location new_node_loc = db_store_->allocIndex();
    BPlusNode new_node(new_node_loc, db_store_);
    new_node.asLeaf();
    new_node.init();
    int mid = TreeOrder / 2;

    // after the new key-value is inserted, copy [mid, ...) to the new node
    if (index == mid) {
        rcopyKV(&leaf, mid, &new_node, 1);
        new_node.addKey(0, key_loc);
        new_node.addValue(0, value_loc);
    } else if (index < mid) {
        rcopyKV(&leaf, mid - 1, &new_node, 0);
        rcopyKV(&leaf, index, &leaf, index + 1);
        leaf.addKey(index, key_loc);
        leaf.addValue(index, value_loc);
    } else { // index > mid
        rcopyKV(&leaf, index, &new_node, index - mid + 1);
        rcopyKV(&leaf, mid, &new_node, 0);
        new_node.addKey(index - mid, key_loc);
        new_node.addValue(index - mid, value_loc);
    }
    leaf.setNext(new_node_loc);
    return new_node_loc;
}

void BPlus::rcopyKV(BPlusNode *src, int isrc, BPlusNode *dest, int idest) {
    int i = TreeOrder - 1;

    while (--i >= isrc && src->getKeyLoc(i).isNull());

    int n = i - isrc + 1;
    assert(n >= 0);

    while (--n >= 0) {
        dest->addKey(idest + n, src->getKeyLoc(isrc + n));
        dest->addValue(idest + n, src->getValueLoc(isrc + n));
        src->removeKey(isrc + n);
        src->removeValue(isrc + n);
    }
}

void BPlus::rcopyK(BPlusNode *src, int isrc, BPlusNode *dest, int idest) {
    int i = TreeOrder - 1;
    while (--i >= isrc && src->getKeyLoc(i).isNull());

    int n = i - isrc + 1;
    assert(n >= 0);
    while (--n >= 0) {
        dest->addKey(idest + n, src->getKeyLoc(isrc + n));
        src->removeKey(isrc + n);
    }
}

void BPlus::rcopyC(BPlusNode *src, int isrc, BPlusNode *dest, int idest) {
    int i = TreeOrder;
    while (--i >= isrc && src->getChildLoc(i).isNull());

    int n = i - isrc + 1;
    assert(n >= 0);
    while (--n >= 0) {
        dest->addChild(idest + n, src->getChildLoc(isrc + n));
        src->removeChild(isrc + n);
    }
}

void BPlus::insertAtInternal(Location key, Location child, std::stack<Location> *parents) {
    while (true) {
        if (parents->empty()) {
            makeNewRoot(key, child);
            break;
        }

        Location parent_loc = parents->top();
        parents->pop();
        BPlusNode parent(parent_loc, db_store_);
        int index = parent.upperBound(key, false);

        if (!parent.isFull()) {
            assert(index >= 0 && index < TreeOrder);
            rcopyK(&parent, index, &parent, index + 1);
            rcopyC(&parent, index + 1, &parent, index + 2);
            parent.addKey(index, key);
            parent.addChild(index + 1, child);
            break;
        }

        Location new_node_loc = db_store_->allocIndex();
        BPlusNode new_node(new_node_loc, db_store_);
        new_node.asInternal();
        new_node.init();

        parent = BPlusNode(parent_loc, db_store_); // data pointer of parent may be deallocated
        int mid = TreeOrder / 2;

        if (index == mid) {
            rcopyK(&parent, mid, &new_node, 0);
            rcopyC(&parent, mid + 1, &new_node, 1);
            new_node.addChild(0, child);
            // key = key
            child = new_node_loc;
        } else if (index < mid) {
            rcopyK(&parent, mid, &new_node, 0);
            rcopyC(&parent, mid, &new_node, 0);
            rcopyK(&parent, index, &parent, index + 1);
            rcopyC(&parent, index + 1, &parent, index + 2);
            parent.addKey(index, key);
            parent.addChild(index + 1, child);
            key = parent.getKeyLoc(mid);
            parent.removeKey(mid);
            child = new_node_loc;
        } else {
            rcopyK(&parent, index, &new_node, index - mid);
            rcopyC(&parent, index + 1, &new_node, index - mid + 1);
            rcopyK(&parent, mid + 1, &new_node, 0);
            rcopyC(&parent, mid + 1, &new_node, 0);
            new_node.addKey(index - mid - 1, key);
            new_node.addChild(index - mid, child);

            key = parent.getKeyLoc(mid);
            parent.removeKey(mid);
            child = new_node_loc;
        }
    }
}

void BPlus::makeNewRoot(const Location &key, const Location &child) {
    Location new_root_loc = db_store_->allocIndex();
    BPlusNode new_root(new_root_loc, db_store_);
    new_root.asInternal();
    new_root.init();
    new_root.addKey(0, key);
    new_root.addChild(0, root_);
    new_root.addChild(1, child);
    db_store_->setRoot(new_root_loc);
    root_ = new_root_loc;
}

void BPlus::remove(const char *key) {
    std::stack<Location> parents;
    BPlusNode leaf(findLeaf(key, &parents), db_store_);
    int index = leaf.find(key);
    if (index < 0) {
        throw NotFoundException();
    }

    // DO NOT DELETE KEY as key is still in the tree
    //db_store_->removeData(leaf.getKeyLoc(index));

    db_store_->removeData(leaf.getValueLoc(index));
    leaf.removeKey(index);
    leaf.removeValue(index);

    lcopyKV(&leaf, index + 1, &leaf, index);

}


void BPlus::lcopyKV(BPlusNode *src, int isrc, BPlusNode *dest, int idest) {
    Location loc;
    if (isrc >= TreeOrder - 1)
        return;
    loc = src->getKeyLoc(isrc);
    while (!loc.isNull()) {
        dest->addKey(idest, loc);
        dest->addValue(idest, src->getValueLoc(isrc));
        src->removeKey(isrc);
        src->removeValue(isrc);
        ++isrc;
        ++idest;
        if (isrc == TreeOrder - 1) break;
        loc = src->getKeyLoc(isrc);
    }
}


}