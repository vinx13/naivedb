#include "lrumap.h"


namespace naivedb {


void LruMap::put(const Key &key, const Value value) {
    auto it = items_map_.find(key);
    if (it != items_map_.end()) {
        items_.erase(it->second);
    }
    items_.emplace_front(key, value);
    items_map_[key] = items_.begin();
}

LruMap::Value LruMap::get(const Key &key) {
    auto it = items_map_.find(key);
    if (it == items_map_.end()) {
        return nullptr;
    }
    items_.splice(items_.begin(), items_, it->second); // move to head of list
    return it->second->second;
}

LruMap::Pair LruMap::removeLru() {
    assert(!items_.empty());
    auto pair = items_.back();
    items_map_.erase(pair.first);
    items_.pop_back();
    return pair;
}


void LruMap::erase(const Key &key) {
    auto it = items_map_.find(key);
    assert(it != items_map_.end());
    items_.erase(it->second);
    items_map_.erase(it);
}
}