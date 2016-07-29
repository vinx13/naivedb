#include <gtest/gtest.h>
#include <cstdlib>
#include <cstring>

#include "dbstore.h"
#include "bplus.h"

using namespace naivedb;

class BPlusNodeTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        MemoryMappedFile::setMemoryLimitation(1024*1024*1024);
    }

};

class BPlusTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        MemoryMappedFile::setMemoryLimitation(1024*1024*1024);
    }

};

TEST_F(BPlusNodeTest, GetKey) {
    DBStore db_store(std::tmpnam(nullptr));
    const char key[] = "this is key";
    Location loc = db_store.saveData(reinterpret_cast<const void *>(key), std::strlen(key) + 1);
    BPlusNode node(db_store.allocIndex(), &db_store);
    node.asLeaf();
    node.init();
    node.addKey(0, loc);
    const char *key2 = node.getKey(0);
    EXPECT_EQ(0, std::strcmp(key, key2));

}

TEST_F(BPlusNodeTest, Find) {
    const char *key1 = "100", *key2 = "200", *key3 = "300", *key4 = "400";

    DBStore db_store(std::tmpnam(nullptr));

    Location loc1 = db_store.saveData(reinterpret_cast<const void *>(key1), std::strlen(key1) + 1),
        loc2 = db_store.saveData(reinterpret_cast<const void *>(key2), std::strlen(key2) + 1),
        loc3 = db_store.saveData(reinterpret_cast<const void *>(key3), std::strlen(key3) + 1),
        loc_index = db_store.allocIndex();

    BPlusNode node(loc_index, &db_store);
    node.asLeaf();
    node.init();
    node.addKey(0, loc1);
    node.addKey(1, loc2);
    node.addKey(2, loc3);
    EXPECT_TRUE(node.find(key4) < 0);
    EXPECT_EQ(0, node.find(key1));
    EXPECT_EQ(2, node.find(key3));
    EXPECT_EQ(1, node.upperBound(key1, true));
    EXPECT_EQ(0, node.upperBound(key1, false));
    EXPECT_EQ(3, node.upperBound(key4, true));

}

TEST_F(BPlusTest, SimpleTest) {
    std::srand(0);
    DBStore db_store(std::tmpnam(nullptr));
    BPlus bplus(&db_store);
    std::map<std::string, int> items;

    int n = 1024000;
    for (int i = 0; i < n; i++) {
        std::string k = std::to_string(rand());

        int v = rand();

        items[k] = v;
        bplus.set(k.c_str(), reinterpret_cast<const void *>(&v), sizeof(v), true);
    }
    char buf[1024];

    for (auto &s:items) {
        int len;
        EXPECT_NO_THROW((len = bplus.get(s.first.c_str(), reinterpret_cast<void *>(buf))));
        EXPECT_EQ(len, sizeof(int));
        EXPECT_EQ(0, std::memcmp(buf, &s.second, len));
    }

    for (auto &pair:items) {
        EXPECT_NO_THROW(bplus.remove(pair.first.c_str()));
    }
    for (auto &pair:items) {
        EXPECT_THROW( bplus.get(pair.first.c_str(), reinterpret_cast<void *>(buf)), NotFoundException);
    }
}


