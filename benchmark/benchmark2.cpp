#include <benchmark/benchmark.h>
#include <cstdlib>
#include <map>
#include <iostream>
#include <random>
#include <set>

#include "database.h"
#include "util.h"

using namespace naivedb;

using namespace std::chrono;

double total = 0;

#define TIMED_TEST_BEGIN \
    do{ steady_clock::time_point t1 = steady_clock::now();

#define TIMED_TEST_END(TEST_NAME) \
    steady_clock::time_point t2 = steady_clock::now(); \
    auto time = duration_cast<duration<double>>(t2 - t1).count(); \
    total+=time; \
    std::cout << TEST_NAME << " finished in " << time << " seconds. " << std::endl; \
} while(0);


void MixedTest(int nrec) {

/*
(1) 向数据库写nrec条记录。
(2) 通过关键字读回nrec条记录。
(3) 执行下面的循环nrec×5次。
(a) 随机读一条记录。
(b) 每循环37次，随机删除一条记录。
(c) 每循环11次，随机添加一条记录并读取这条记录。
(d) 每循环17次，随机替换一条记录为新记录。在连续两次替换中，一次用同样大小的记录替换，一次用比以前更长的记录替换。
(4) 将此进程写的所有记录删除。每删除一条记录，随机地寻找10条记录。
 */


    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    const int KeySize = 16;
    const int ValueSize = 100;

    DatabaseOption option;
    option.memory_limitation = 1024 * 1024 * 1024;
    Database database(std::tmpnam(nullptr), option);

    std::vector<std::string> keys;
    std::set<std::string> values;

    char *buf = new char[ValueSize * 2];

    int ikey = 0;
    for (; ikey < nrec; ikey++) {
        std::string key = numberToString(ikey, KeySize - 1);
        keys.push_back(key);
    }
    for (int i = 0; i < nrec * 2; i++) { values.emplace(generateRandomString(ValueSize)); }
    auto iter_value = values.begin();

    std::shuffle(keys.begin(), keys.end(), generator);


    // Test 1
    TIMED_TEST_BEGIN
    for (const auto &key:keys) {
        database.set(key, (++iter_value)->c_str(), ValueSize, false);
    }
    TIMED_TEST_END("Test 1")


    std::shuffle(keys.begin(), keys.end(), generator);

    // Test 2
    TIMED_TEST_BEGIN
    for (const auto &key:keys) {
        database.get(key, buf);
    }
    TIMED_TEST_END("Test 2")


    // Test 3
    TIMED_TEST_BEGIN
    for (int i = 0; i < nrec * 5; i++) {
        std::uniform_int_distribution<int> dist(0, keys.size() - 1);
        const std::string key_to_search = keys.at(dist(generator));

        // (a)
        database.get(key_to_search, buf);

        // (b)
        if (i % 37 == 0) {
            std::uniform_int_distribution<int> dist(0, keys.size() - 1);
            int index = dist(generator);
            const std::string key_to_delete = keys[index];
            keys[index] = keys.back();
            keys.pop_back();

            database.remove(key_to_delete);
        }

        // (c)
        if (i % 11 == 0) {
            const std::string key_to_insert = numberToString(ikey++, KeySize - 1);

            keys.push_back(key_to_insert);

            database.set(key_to_insert, (++iter_value)->c_str(), ValueSize, false);
            database.get(key_to_insert, buf);
        }

        // (d)
        if (i % 17 == 0) {
            std::uniform_int_distribution<int> dist(0, keys.size() - 1);
            const std::string key = keys.at(dist(generator));

            int len = ValueSize;
            if (i % 34 == 0)len *= 2;

            const std::string value = generateRandomString(len);

            database.set(key, value.c_str(), len, true);
        }

    }
    TIMED_TEST_END("Test 3")

    std::shuffle(keys.begin(), keys.end(), generator);

    // Test 4
    TIMED_TEST_BEGIN
    for (int i = 0; i < keys.size(); i++) {
        const std::string &key = keys[i];

        database.remove(key);

        if (i == keys.size() - 1)break; // no more keys for query
        for (int j = 0; j < 10; j++) {
            std::uniform_int_distribution<int> dist(i + 1, keys.size() - 1);
            const std::string key_to_search = keys.at(dist(generator));

            database.get(key_to_search, buf);
        }
    }
    TIMED_TEST_END("Test 4")

    delete[] buf;
}

int main() {
    int nrec = 2;
    int max_nrec = 1 << 21;
    while(nrec < max_nrec) {
        total = 0;
        int iteration = 1000;
        for(int i = 0; i < iteration; i++) {
            MixedTest(nrec);
        }
        double mean_time = total / iteration;
        std::cout << "nrec = " << nrec << "    time = " << mean_time << std::endl;
        nrec <<= 1;
    }
    return  0;
}