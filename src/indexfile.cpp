#include <cassert>

#include "indexfile.h"
#include "dbstore.h"
#include "storage.h""

namespace naivedb {


const int DefaultIndexFileSize = 1024 * 1024;

IndexFile::IndexFile(const std::string &filename, DBStore *db_store) : File(filename, db_store) {

}

IndexFileHeader *IndexFile::getHeader() {
    return reinterpret_cast<IndexFileHeader *>(file_.get());
}

IndexRecord *IndexFile::recordAt(int offset) {
    assert(offset > 0);
    return reinterpret_cast<IndexRecord *>(file_.get(offset));
}

int IndexFile::alloc() {
    int head_ofs = getHeader()->empty_index_node_ofs;
    IndexRecord *empty_head = recordAt(head_ofs);
    int next_ofs = empty_head->next;

    if (next_ofs < 0) {
        grow();
    } else {
        getHeader()->empty_index_node_ofs = next_ofs;
    }
    return head_ofs;
}

int IndexFile::getBucketIndex(int min_size) const {
    for (int i = 0; i < NumBucket; i++) {
        if (BucketSizes[i] >= min_size)
            return i;
    }
    return NumBucket - 1;
}

void IndexFile::reserve(int min_size) {
    int size = getHeader()->index_file_size;
    if (size >= min_size)
        return;
    for (; size < min_size; size <<= 1);
    file_.grow(size);
}

void IndexFile::collect(int offset) {
    int *next = static_cast<int *>(recordAt(offset)->getData());
    IndexFileHeader *header = getHeader();
    *next = header->empty_index_node_ofs;
    header->empty_index_node_ofs = offset;
}

bool File::isExist() {
    return file_.isExist();
}


void IndexFile::initHeader() {
    IndexFileHeader *header = getHeader();
    for (int i = 0; i < NumBucket; i++) {
        header->empty_heads[i].init();
    }
    header->empty_index_node_ofs = sizeof(*header);
    header->index_file_size = DefaultIndexFileSize; // FIXME
    header->num_data_files = 0;
    header->tree_root = -1;
}

void IndexFile::addToEmptyHeads(const Location &loc) {
    assert(!loc.isNull());
    IndexFileHeader *header = getHeader();
    DataRecord *record = db_store_->dataRecordAt(loc);
    int bucket = getBucketIndex(record->block_size);
    record->next = header->empty_heads[bucket];
}


Location IndexFile::getEmptyLocation(int bucket) {
    assert(bucket > 0 && bucket < NumBucket);
    return getHeader()->empty_heads[bucket];
}

void IndexFile::removeEmptyLocation(int bucket) {
    IndexFileHeader *header = getHeader();
    assert(!header->empty_heads[bucket].isNull());
    header->empty_heads[bucket] = db_store_->dataRecordAt(header->empty_heads[bucket])->next;
}

void IndexFile::grow() {
    IndexFileHeader *header = getHeader();
    int cur_size = header->index_file_size;
    int new_size = cur_size << 1;
    if (new_size <= 0) {
        // TODO
        return;
    }
    file_.grow(new_size);
    header->index_file_size = new_size;
    int i = header->empty_index_node_ofs;
    IndexRecord *record = recordAt(i);
    while (record->next > 0) {
        // go to end of the list
        i = record->next;
        record = recordAt(i);
    }
    record->next = cur_size;
    initEmptyRecords(cur_size);


}

void IndexFile::initEmptyRecords(int index) {

    IndexFileHeader *header = getHeader();
    const int record_size = sizeof(IndexRecord),
        size = header->index_file_size;
    while (index < size) {
        IndexRecord *record = recordAt(index);
        int next_ofs = index + record_size;
        record->next = next_ofs < size ? next_ofs : -1;
        index += record_size;
    }
}

void IndexFile::init() {
    initHeader();
    IndexFileHeader *header = getHeader();
    int i = header->empty_index_node_ofs;
    initEmptyRecords(i);
}


}