#ifndef NAIVEDB_LRUMAP_H
#define NAIVEDB_LRUMAP_H

#include <cassert>
#include <list>
#include <string>
#include <utility>
#include <unordered_map>

#include "platform.h"

namespace naivedb {

// value type must be a pointer because nullptr is returned if search failure occurs
template<class Key, class Value, class=typename std::enable_if<std::is_pointer<Value>::value, bool>::type>
class LruMap {
public:

    typedef std::pair<Key, Value> Pair;
    typedef std::list<Pair> Items;

    void put(const Key &key, const Value value) {
        auto it = items_map_.find(key);
        if (it != items_map_.end()) {
            items_.erase(it->second);
        }
        items_.emplace_front(key, value);
        items_map_[key] = items_.begin();
    }

    Value get(const Key &key) {
        auto it = items_map_.find(key);
        if (it == items_map_.end()) {
            return nullptr;
        }
        items_.splice(items_.begin(), items_, it->second); // move to head of list
        return it->second->second;
    }

    Pair removeLru() {
        assert(!items_.empty());
        auto pair = items_.back();
        items_map_.erase(pair.first);
        items_.pop_back();
        return pair;
    }

    void erase(const Key &key) {
        auto it = items_map_.find(key);
        assert(it != items_map_.end());
        items_.erase(it->second);
        items_map_.erase(it);
    }

private:

    std::unordered_map<Key, typename Items::iterator> items_map_;
    Items items_;
};


}
#endif //NAIVEDB_LRUMAP_H
