#include <gtest/gtest.h>
#include <datafile.h>
#include <exception.h>

#include "storage.h"
#include "dbstore.h"
#include "indexfile.h"

using namespace naivedb;

int countRecords(DBStore *db_store, int start) {
    int count = 1;
    IndexRecord *record = db_store->indexRecordAt(start);
    while (record->next > 0) {
        ++count;
        record = db_store->indexRecordAt(record->next);
    }
    return count;
};

TEST(StorageTest, InitIndexFile) {
    DBStore db_store(std::tmpnam(nullptr));
    int ofs = db_store.getIndexFile()->getHeader()->empty_index_node_ofs;
    int count = 1;

    int num_records = countRecords(&db_store, ofs);
    int expect = (DefaultIndexFileSize - sizeof(IndexFileHeader)) / sizeof(IndexRecord);
    EXPECT_EQ(expect, num_records);
}

TEST(StorageTest, AllocIndex) {
    DBStore db_store(std::tmpnam(nullptr));
    int index = db_store.allocIndex();
    EXPECT_EQ(sizeof(IndexFileHeader), index);
    EXPECT_EQ(index + sizeof(IndexRecord), db_store.getIndexFile()->getHeader()->empty_index_node_ofs);
}

TEST(StorageTest, CollectIndex) {
    DBStore db_store(std::tmpnam(nullptr));
    int first = db_store.getIndexFile()->getHeader()->empty_index_node_ofs;
    db_store.collectIndexSpace(db_store.allocIndex());
    EXPECT_EQ(first, db_store.getIndexFile()->getHeader()->empty_index_node_ofs);
    EXPECT_EQ(first, db_store.allocIndex());
}

TEST(StorageTest, GrowIndexFile) {
    DBStore db_store(std::tmpnam(nullptr));
    int ofs = db_store.getIndexFile()->getHeader()->empty_index_node_ofs;
    db_store.getIndexFile()->grow();
    int num_records = countRecords(&db_store, ofs);
    int expect = (DefaultIndexFileSize - sizeof(IndexFileHeader)) / sizeof(IndexRecord)
                 + DefaultIndexFileSize / sizeof(IndexRecord);
}

TEST(StorageTest, AllocData) {
    DBStore db_store(std::tmpnam(nullptr));
    EXPECT_THROW(db_store.allocData(DefaultDataFileSize * 2), SizeLimitException);
    Location loc;
    EXPECT_NO_THROW((loc = db_store.allocData(1024)));
    EXPECT_EQ(0,loc.file_no);
    EXPECT_EQ(0,loc.offset);
}

TEST(StorageTest, CollectData) {
    DBStore db_store(std::tmpnam(nullptr));
    const int size = 1024;
    Location loc = db_store.allocData(size);
    db_store.collectDataSpace(loc);
    Location loc2 = db_store.allocData(size);
    EXPECT_EQ(loc2.file_no, loc.file_no);
    EXPECT_EQ(loc2.offset, loc.offset);
}