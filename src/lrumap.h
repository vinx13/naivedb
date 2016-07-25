#ifndef NAIVEDB_LRUMAP_H
#define NAIVEDB_LRUMAP_H

#include <cassert>
#include <list>
#include <string>
#include <utility>
#include <unordered_map>


namespace naivedb {


class LruMap {

    typedef std::string Key;
    typedef void *Value;
    typedef std::pair<Key, Value> Pair;
    typedef std::list<Pair> Items;

public:

    void put(const Key &key, const Value value);

    Value get(const Key &key);

    Value removeLru();

private:

    std::unordered_map<Key, Items::iterator> items_map_;
    Items items_;
};


}
#endif //NAIVEDB_LRUMAP_H
