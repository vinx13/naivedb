#include <gtest/gtest.h>
#include <random>
#include <chrono>
#include <algorithm>

#include "lrumap.h"

using namespace naivedb;

TEST(LruMapTest, SimpleInsert) {
    LruMap map;
    map.put(1, (void *) 1);
    EXPECT_EQ((void *) 1, map.get(1));
    EXPECT_EQ(nullptr, map.get(2));
    map.put(3, (void *) 3);
    EXPECT_EQ((void *) 1, map.get(1));
    EXPECT_EQ((void *) 3, map.get(3));
    map.put(3, (void *) 5);
    EXPECT_EQ((void *) 5, map.get(3));
    EXPECT_EQ(1, map.removeLru().first);
}

TEST(LruMapTest, RemoveLru) {
    LruMap map;
    std::vector<int> elems;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    for (int i = 1; i < 100; i++) {
        elems.push_back(i);
        map.put(i, (void *) i);
    }
    std::shuffle(elems.begin(), elems.end(), std::default_random_engine(seed));

    for (int i : elems) {
        EXPECT_EQ((void *) i, map.get(i));
    }
    for (int i : elems) {
        EXPECT_EQ((void *) i, map.removeLru().second);
    }

}
