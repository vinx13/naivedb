#ifndef NAIVEDB_LRUMAP_H
#define NAIVEDB_LRUMAP_H

#include <cassert>
#include <list>
#include <string>
#include <utility>
#include <unordered_map>


namespace naivedb {


class LruMap {
public:

    typedef int Key;
    typedef void *Value;
    typedef std::pair<Key, Value> Pair;
    typedef std::list<Pair> Items;

    void put(const Key &key, const Value value);

    Value get(const Key &key);

    Pair removeLru();

    void erase(const Key &key);

private:

    std::unordered_map<Key, Items::iterator> items_map_;
    Items items_;
};


}
#endif //NAIVEDB_LRUMAP_H
