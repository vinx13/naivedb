#include <gtest/gtest.h>
#include <datafile.h>
#include "dbstore.h"
#include "indexfile.h"

using namespace naivedb;

TEST(IndexFileTest, Alloc) {
    IndexFileMgr file_mgr(std::tmpnam(nullptr));
    file_mgr.init();
    Location loc = file_mgr.alloc();
    EXPECT_EQ(1, loc.file_no);
    EXPECT_EQ(0, loc.offset);
    Location loc2 = file_mgr.alloc();
    EXPECT_EQ(1, loc2.file_no);
    EXPECT_EQ(sizeof(IndexRecord), loc2.offset);
}

TEST(IndexFileTest, Collect) {
    IndexFileMgr file_mgr(std::tmpnam(nullptr));
    file_mgr.init();
    Location loc = file_mgr.alloc();
    file_mgr.collect(loc);
    loc = file_mgr.alloc();
    EXPECT_EQ(1, loc.file_no);
    EXPECT_EQ(0, loc.offset);
}

TEST(DataFileTest, Alloc) {
    DataFileMgr file_mgr(std::tmpnam(nullptr));
    file_mgr.init();
    EXPECT_THROW(file_mgr.alloc(DefaultDataFileSize * 2).isNull(), SizeLimitException);

    Location loc;
    EXPECT_NO_THROW((loc = file_mgr.alloc(1024)));
    EXPECT_FALSE(loc.isNull());
    DataRecord *record= file_mgr.recordAt(loc);
    EXPECT_EQ(1024,record->data_size);
    EXPECT_TRUE(record->block_size >= 1024);
    EXPECT_EQ(1,loc.file_no);
    EXPECT_EQ(0,loc.offset);
}

TEST(DataFileTest, Collect) {
    DataFileMgr file_mgr(std::tmpnam(nullptr));
    file_mgr.init();
    const int size = 1024;
    Location loc = file_mgr.alloc(size);
    file_mgr.collect(loc);
    Location loc2 = file_mgr.alloc(size);
    EXPECT_EQ(loc2.file_no, loc.file_no);
    EXPECT_EQ(loc2.offset, loc.offset);
}

TEST(DBStoreTest, Save) {
    int magic1 = 0x01314242, magic2 = 0x02313144;
    DBStore db_store(std::tmpnam(nullptr));

    Location loc1 = db_store.saveData(reinterpret_cast<void *>(&magic1), sizeof(magic1)),
        loc2=db_store.saveData(reinterpret_cast<void *>(&magic2), sizeof(magic2));
    EXPECT_EQ(0,std::memcmp(db_store.dataRecordAt(loc1)->getData(), reinterpret_cast<void *>(&magic1), sizeof(magic1)));
    EXPECT_EQ(0,std::memcmp(db_store.dataRecordAt(loc2)->getData(), reinterpret_cast<void *>(&magic2), sizeof(magic2)));
}