#include "datafile.h"

namespace naivedb {

const int DefaultDataFileSize = 1024*1024;

DataFile::DataFile(const std::string &filename, int file_no, DBStore *db_store) :
    File(filename, db_store), file_no_(file_no) {
    if(~file_.isExist()){
        file_.create(DefaultDataFileSize);
    } else {
        file_.open();
    }
}

void DataFile::init() {
    DataRecord *first = recordAt(0);
    first->block_size = DefaultDataFileSize;
}

bool DataFile::isReusableSize(int size) const {
    return size >= BucketSizes[0];
}

DataRecord *DataFile::recordAt(int offset) {
    assert(offset > 0);
    return reinterpret_cast<DataRecord *>(file_.get(offset));
}

int DataFile::allocAt(int offset, int size) {
    assert(size > 0);
    DataRecord *record = recordAt(offset);
    int remained_size = record->block_size - size;

    if (!isReusableSize(remained_size))return -1;
    int remained_offset = offset + sizeof(record->block_size) + size;
    recordAt(remained_offset)->block_size = remained_size - sizeof(record->block_size);
    return remained_offset;
}

void DataFile::collect(int offset, int size) {
    assert(offset > 0);
    assert(size > 0);
}


}