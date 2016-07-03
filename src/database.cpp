#include "database.h"

namespace medb {

Database::Database(const std::string &database) : db_name_(database) {
    db_store_ = new DBStore(database);
    tree_ = new BPlus(db_store_);

}

Database::~Database() {
    delete db_store_;
    delete tree_;
}

void Database::get(const char *key, void **value, int *len) {
    tree_->get(key, value, len);
}

void Database::set(const char *key, const void *value, int len, bool overwrite = false) {
    tree_->set(key, value, len, overwrite);
}

void Database::remove(const char *key) {
    tree_->remove(key);
}


}