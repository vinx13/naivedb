#include <cassert>

#include "file.h"
#include "mmap.h"

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
        collectDataSpace(loc);
    }
    // otherwise:
    // remained size is too small, so the whole block is allocated
    // leaving record->block_size unchanged

    return loc;
}

void FileMgr::collectDataSpace(const Location &loc) {
    assert(!loc.isNull());
    IndexFileHeader *header = getIndexFileHeader();
    DataRecord *record = dataRecordAt(loc);
    int bucket = getBucketIndex(record->block_size);
    record->next = header->empty_heads[bucket];
}

void FileMgr::collectIndexSpace(int offset) {
    int *next = static_cast<int *>(indexRecordAt(offset)->getData());
    IndexFileHeader *header = getIndexFileHeader();
    *next = header->empty_index_node_ofs;
    header->empty_index_node_ofs = offset;
}


int FileMgr::allocIndex() {
    int head_ofs = getIndexFileHeader()->empty_index_node_ofs;
    IndexRecord *empty_head = indexRecordAt(head_ofs);
    int next_ofs = static_cast<int>(empty_head->data);

    if (next_ofs < 0) {
        //TODO grow index file
    } else {
        getIndexFileHeader()->empty_index_node_ofs = next_ofs;
    }
    return head_ofs;
}

void FileMgr::removeFragments() {
    // TODO
}

void FileMgr::reserveIndexSpace(int min_size) {
    int size = getIndexFileHeader()->index_file_size;
    if (size >= min_size)
        return;
    for (; size < min_size; size <<= 1);
    MemoryMapper::getInstance()->grow(getIndexFileName(), min_size);
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
    IndexFileHeader *header = getIndexFileHeader();
    header->num_data_files = 0;
    // TODO init header
    for (int i = 0; i < NumBucket; i++) {
        header->empty_heads[i].file_no = -1;
    }
}

void *FileMgr::openFile(const std::string &filename) const {
    void *p = MemoryMapper::getInstance()->open(filename);
    assert(p);
    return p;
}

int FileMgr::getBucketIndex(int min_size) {
    IndexFileHeader *header = getIndexFileHeader();
    for (int i = 0; i < NumBucket; i++) {
        if (BucketSizes[i] >= min_size &&
            !header->empty_heads[i].isNull() &&
            dataRecordAt(header->empty_heads[i])->block_size > min_size) {
            return i;
        }
    }
    return -1;
}

DataRecord *FileMgr::dataRecordAt(const Location &loc) {
    assert(!loc.isNull());
    void *f = getDataFile(loc.file_no);
    return reinterpret_cast<DataRecord *>(static_cast<char *>(f) + loc.offset);
}

IndexRecord *FileMgr::indexRecordAt(int offset) {
    assert(offset > 0);
    void *f = getIndexFile();
    return reinterpret_cast<IndexRecord *>(static_cast<char *>(f) + offset);
}

bool FileMgr::isReusableSize(int size) const {
    return size >= BucketSizes[0];
}

IndexFileHeader *FileMgr::getIndexFileHeader() {
    return reinterpret_cast<IndexFileHeader *>(getIndexFile());
}


}
