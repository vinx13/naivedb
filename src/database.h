#ifndef NAIVEDB_DATABASE_H
#define NAIVEDB_DATABASE_H

#include <string>

namespace naivedb {

class DBStore;

class BPlus;

class Database {

public:

    Database(const std::string &database);

    ~Database();

    int get(const std::string &key, void *value);

    void set(const std::string &key, const void *value, int len, bool overwrite);

    void remove(const std::string &key);

    template<class T, class = typename std::enable_if<std::is_pod<T>::value, bool>::type>
    T get(const std::string &key) {
        T t;
        get(key, reinterpret_cast<void *>(&t));
        return t;
    }

    template<class T, class = typename std::enable_if<std::is_pod<T>::value, bool>::type>
    void set(const std::string &key, const T &value, bool overwrite) {
        set(key, reinterpret_cast<void *>(&value), sizeof(T), overwrite);
    };

private:

    Database(
        const Database &) = delete;

    Database &operator=(const Database &) = delete;

    const std::string db_name_;
    DBStore *db_store_;
    BPlus *bplus_;

};


}

#endif //NAIVEDB_DATABASE_H
