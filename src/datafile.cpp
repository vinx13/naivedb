#include "datafile.h"

namespace naivedb {


DataFileMgr::DataFileMgr(const std::string &filename) : FileMgr(filename, DefaultDataFileSize) {
}

void DataFileMgr::initFile(int file_no) {
    DataRecord *first = recordAt({file_no, 0});
    first->block_size = DefaultDataFileSize - sizeof(first->block_size);
    addToHead({file_no, 0});
}

void DataFileMgr::initHeader() {
    DataFileHeader *header = getDataHeader();
    header->num_files = 1;
    for (auto &head:header->empty_heads) {
        head.init();
    }
}

bool DataFileMgr::isReusableSize(int size) const {
    return size >= BucketSizes[0];
}

DataRecord *DataFileMgr::recordAt(const Location &location) {
    return reinterpret_cast<DataRecord *>(get(location));
}

int DataFileMgr::allocAt(const Location &location, int size) {
    assert(size > 0);
    DataRecord *record = recordAt(location);
    int remained_size = record->block_size - size;

    if (!isReusableSize(remained_size))return -1;
    int remained_offset = location.offset + sizeof(record->block_size) + size;
    record->block_size = size;
    recordAt({location.file_no,remained_offset})->block_size = remained_size - sizeof(record->block_size);
    return remained_offset;
}

void DataFileMgr::collect(const Location &location) {
    assert(!location.isNull());
    addToHead(location);
}


DataFileHeader *DataFileMgr::getDataHeader() {
    return reinterpret_cast<DataFileHeader *>(getFileHeader());
}

void DataFileMgr::removeEmptyLocation(int bucket) {
    DataFileHeader *header = getDataHeader();
    assert(!header->empty_heads[bucket].isNull());
    header->empty_heads[bucket] = recordAt(header->empty_heads[bucket])->next;
}

Location DataFileMgr::getFreeLocation(int size_to_alloc) {
    int bucket = getSuggestedBucketFetch(size_to_alloc);

    Location loc;
    while (bucket < NumBucket) {
        loc = getFromHead(bucket);
        if (loc.isNull())
            ++bucket;
        else
            break;
    }
    if (bucket == NumBucket) {
        createFile();
        bucket = NumBucket - 1;
    }
    if (bucket == NumBucket - 1) {
        // the size of last bucket is not limited, so either head of the last bucket is not null or a new file is created
        loc = getFromHead(bucket);
        assert(!loc.isNull());
        int available_size = recordAt(loc)->block_size;
        if (available_size < size_to_alloc) {
            return Location::None;
        }
    }
    removeEmptyLocation(bucket);
    return loc;
}

Location DataFileMgr::alloc(int min_size) {
    assert(min_size > 0);
    int size_to_alloc = (min_size + 3) & 0xfffffffc; // make it multiple of 4 for better performance
    Location loc = getFreeLocation(size_to_alloc);

    if(loc.isNull()){
        throw SizeLimitException();
    }

    int block_size = recordAt(loc)->block_size;
    assert(size_to_alloc <= block_size);

    int remained_offset = allocAt(loc, size_to_alloc);
    recordAt(loc)->data_size = min_size; // min_size is size of data, which may be less than size_to_alloc
    if (remained_offset >= 0) {
        addToHead({loc.file_no, remained_offset});
    }
    return loc;
}

void DataFileMgr::addToHead(const Location &loc) {
    assert(!loc.isNull());
    DataFileHeader *header = getDataHeader();
    DataRecord *record = recordAt(loc);

    int bucket = getSuggestedBucketPut(record->block_size);

    assert(bucket >= 0);
    record->next = header->empty_heads[bucket];
    header->empty_heads[bucket] = loc;
}

int DataFileMgr::getSuggestedBucketFetch(int min_size) const {
    for (int i = 0; i < NumBucket; i++) {
        if (BucketSizes[i] >= min_size)
            return i;
    }
    return NumBucket - 1;
}

int DataFileMgr::getSuggestedBucketPut(int size) {
    int i;
    for(i = 0 ; i <NumBucket; i++){
        if(BucketSizes[i] > size)
            break;
    }
    return --i;
}

Location DataFileMgr::getFromHead(int bucket) {
    assert(bucket >= 0 && bucket < NumBucket);
    return getDataHeader()->empty_heads[bucket];
}


}