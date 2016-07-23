#include <cassert>
#include <cstdlib>
#include <cstring>

#include "dbstore.h"
#include "indexfile.h"
#include "datafile.h"


namespace naivedb {


DBStore::DBStore(const std::string &database) :
    database_(database),
    index_file_(new IndexFileMgr(getIndexPrefix())),
    data_file_(new DataFileMgr(getDataPrefix())) {

    index_file_->init();
    data_file_->init();
}

std::string DBStore::getIndexPrefix() const { return database_ + ".idx."; }

std::string DBStore::getDataPrefix() const { return database_ + "."; }

DataRecord *DBStore::dataRecordAt(const Location &location) {
    data_file_->recordAt(location);
}

IndexRecord *DBStore::indexRecordAt(const Location &location) {
    index_file_->recordAt(location);
}

void DBStore::saveData(const void *data, int len) {
    Location loc = data_file_->alloc(len);
    DataRecord *record = dataRecordAt(loc);
    record->data_size = len;
    std::memcpy(record->getData(), data,len);
}


Location DBStore::getRoot() {
    return index_file_->getIndexHeader()->tree_root;
}

void DBStore::setRoot(const Location &location) {
    index_file_->getIndexHeader()->tree_root = location;
}

Location DBStore::getFirstLeaf() {
    return index_file_->getIndexHeader()->first_leaf;
}

void DBStore::setFirstLeaf(const Location &location) {
    index_file_->getIndexHeader()->first_leaf = location;
}

Location DBStore::allocIndex() {
    return index_file_->alloc();
}

Location DBStore::allocData(int min_size) {
    return data_file_->alloc(min_size);
}

void DBStore::collectIndex(const Location &location) {
    index_file_->collect(location);
}

void DBStore::collectData(const Location &location) {
    data_file_->collect(location);
}

}