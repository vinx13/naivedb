#include <tclDecls.h>
#include "storage.h"
#include <cassert>

namespace medb {


FileMgr::FileMgr(const std::string &database) : database_(database) {
}

void *FileMgr::getIndexFile() {
    if (!index_file_)
        index_file_ = openFile(getIndexFileName());
    return index_file_;
}

void *FileMgr::getDataFile(int file_no) {
    assert(file_no < 0 || file_no >= data_files_.size());
    if (!data_files_[file_no])
        data_files_[file_no] = openFile(getDataFileName(file_no));
    return data_files_[file_no];
}

Location FileMgr::allocData(int min_size) {
    assert(min_size > 0);

    IndexFileHeader *header = getIndexFileHeader();

    int size_to_alloc = (min_size + 3) & 0xfffffffc; // make it multiple of 4 for better performance
    int bucket = getBucketIndex(size_to_alloc);
    if (bucket < 0) {
        createDataFile();
        bucket = NumBucket - 1;
    }
    return allocDataOn(bucket, size_to_alloc);
}

Location FileMgr::allocDataOn(int bucket, int size) {
    // always assume that the provided location has enough space
    IndexFileHeader *header = static_cast<IndexFileHeader *>getIndexFile();
    Location loc = header->empty_heads[bucket];
    DataRecord *record = dataRecordAt(loc);

    int size_remained = record->block_size - size;
    if (isReusableSize(size_remained)) {
        record->block_size = size;
        Location loc_remained = loc;
        loc_remained.offset += size;
        DataRecord *record_remained = dataRecordAt(loc_remained);
        record_remained->block_size = size_remained;
        collectSpace(loc);
    }
    // otherwise:
    // remained size is too small, so the whole block is allocated
    // leaving record->block_size unchanged

    return record;
}

void FileMgr::collectSpace(const Location &loc) {
    assert(!loc.isNull());
    IndexFileHeader *header = getIndexFileHeader();
    DataRecord *record = dataRecordAt(loc);
    int bucket = getBucketIndex(record->block_size);
    record->next = header->empty_heads[bucket];
}

Location FileMgr::allocIndex(int size) {
    return medb::Location();
}

void FileMgr::removeFragments() {

}

void FileMgr::reserveIndexSpace(int min_size) {

}

std::string FileMgr::getDataFileName(int file_no) const {
    return database_ + "." + std::to_string(file_no);
}

std::string FileMgr::getIndexFileName() const {
    return database_ + ".idx";
}

void FileMgr::createDataFile() {
    getIndexFileHeader()->num_data_files++;
    data_files_.push_back(nullptr);
}

void FileMgr::createIndexFile() {
    IndexFileHeader * header = getIndexFileHeader();
    header->num_data_files = 0;
    // TODO init header
    for(int i = 0; i < NumBucket; i++) {
        header->empty_heads[i].file_no = -1;
    }
}

void *FileMgr::openFile(const std::string &filename) const {
    void *p = nullptr; //TODO
    assert(p);
    return p;
}

int FileMgr::getBucketIndex(int min_size) {
    IndexFileHeader *header = getIndexFileHeader();
    for (int i = 0; i < NumBucket; i++) {
        if (BucketSizes[i] >= min_size &&
            !header->empty_heads[i].isNull() &&
            dataRecordAt(header->empty_heads[i])->block_size>min_size) {
            return i;
        }
    }
    return -1;
}

DataRecord *FileMgr::dataRecordAt(const Location &loc) {
    assert(!loc.isNull());
    void *f = getDataFile(loc.file_no);
    return static_cast<DataRecord *>(f + loc.offset);
}

IndexRecord *FileMgr::indexRecordAt(int offset) {
    assert(offset > 0);
    void *f = getIndexFile();
    return static_cast<IndexRecord *>(f + offset);
}

bool FileMgr::isReusableSize(int size) const {
    return size >= BucketSizes[0];
}

IndexFileHeader *FileMgr::getIndexFileHeader() {
    return static_cast<IndexFileHeader *>(getIndexFile());
}


}
