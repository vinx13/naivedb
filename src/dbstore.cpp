#include <cassert>

#include "dbstore.h"
#include "indexfile.h"
#include "datafile.h"
#include "exception.h"


namespace naivedb {


DBStore::DBStore(const std::string &database) : database_(database) {
    openAllFiles();

}

DBStore::~DBStore() {
    closeAllFiles();
}

void DBStore::openAllFiles() {
    index_file_ = new IndexFile(getIndexFileName(), this);
    int num_files = index_file_->getHeader()->num_data_files;
    for(int i = 0 ; i < num_files; i++) {
        data_files_.push_back(new DataFile(getDataFileName(i), i, this));
    }
}

void DBStore::closeAllFiles() {

    delete index_file_;
    for (auto data_file : data_files_) {
        delete data_file;
    }
}

IndexFile *DBStore::getIndexFile() {
    return index_file_;
}

DataFile *DBStore::getDataFile(int file_no) {
    assert(file_no >= 0 && file_no < data_files_.size());
    return data_files_[file_no]; // file_no starts from zero
}


Location DBStore::allocData(int min_size) {
    assert(min_size > 0);

    int size_to_alloc = (min_size + 3) & 0xfffffffc; // make it multiple of 4 for better performance
    int bucket = index_file_->getBucketIndex(size_to_alloc);
    Location loc;
    while (bucket < NumBucket) {
        loc = index_file_->getEmptyLocation(bucket);
        if (loc.isNull())
            ++bucket;
        else
            break;
    }

    if (bucket == NumBucket) {
        createDataFile();
        bucket = NumBucket - 1;
    }
    if (bucket == NumBucket - 1) {
        // the size of last bucket is not limited, so either head of the last bucket is not null or a new file is created
        loc = index_file_->getEmptyLocation(bucket);
        int available_size = dataRecordAt(loc)->block_size;
        if (available_size < size_to_alloc) {
            throw SizeLimitException();
        }
    }
    int block_size = dataRecordAt(loc)->block_size;
    assert(size_to_alloc <= block_size);

    index_file_->removeEmptyLocation(bucket);

    int remained_offset = data_files_[loc.file_no]->allocAt(0, size_to_alloc);
    dataRecordAt(loc)->data_size = min_size; // min_size is size of data, which may be less than size_to_alloc
    if (remained_offset >= 0) {
        index_file_->addToEmptyHeads(Location(loc.file_no, remained_offset));
    }
    return loc;
}

void DBStore::collectDataSpace(const Location &loc) {
    index_file_->addToEmptyHeads(loc);
}

void DBStore::collectIndexSpace(int offset) {
    index_file_->collect(offset);
}

int DBStore::allocIndex() {
    return index_file_->alloc();
}


std::string DBStore::getDataFileName(int file_no) const {
    return database_ + "." + std::to_string(file_no);
}

std::string DBStore::getIndexFileName() const {
    return database_ + ".idx";
}

int DBStore::createDataFile() {
    IndexFileHeader *header = index_file_->getHeader();
    int file_no = header->num_data_files++;
    data_files_.emplace_back(new DataFile(getDataFileName(file_no), file_no, this));
    Location loc;
    loc.file_no = file_no;
    loc.offset = 0;
    index_file_->addToEmptyHeads(loc);
}

DataRecord *DBStore::dataRecordAt(const Location &loc) {
    return getDataFile(loc.file_no)->recordAt(loc.offset);
}

IndexRecord *DBStore::indexRecordAt(int offset) {
    return index_file_->recordAt(offset);
}


}