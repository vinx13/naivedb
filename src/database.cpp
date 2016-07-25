#include "database.h"
#include "bplus.h"


namespace naivedb {


Database::Database(const std::string &database) : db_name_(database) {
    db_store_ = new DBStore(database);
    bplus_ = new BPlus(db_store_);
}

Database::~Database() {
    delete db_store_;
    delete bplus_;
}

int Database::get(const std::string &key, void *value) {
    return bplus_->get(key.c_str(), value);
}

void Database::set(const std::string &key, const void *value, int len, bool overwrite) {
    bplus_->set(key.c_str(), value, len, overwrite);
}

void Database::remove(const std::string &key) {
    bplus_->remove(key.c_str());
}


}