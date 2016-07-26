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

#include <cstdlib>
#include <map>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include "database.h"
#include "util.h"

using namespace naivedb;

using namespace std::chrono;

#define TIMED_TEST_BEGIN \
    do{ steady_clock::time_point t1 = steady_clock::now();

#define TIMED_TEST_END(TEST_NAME) \
    steady_clock::time_point t2 = steady_clock::now(); \
    std::cout << TEST_NAME << " finished in " << duration_cast<duration<double>>(t2 - t1).count() << " seconds. " << std::endl; \
} while(0);

int main() {

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    const int nrec = 1000000;
    const int ValueSize = 100;
    Database database(std::tmpnam(nullptr));
    std::vector<std::pair<std::string, std::string> > items;
    char buf[2*ValueSize];

    int ikey = 0;
    for (; ikey < nrec; ikey++) {
        std::string key = std::to_string(ikey),
            value = generateRandomString(ValueSize);
        items.emplace_back(key, value);
    }

    std::shuffle(items.begin(), items.end(), std::default_random_engine(seed));

    // Test 1
    TIMED_TEST_BEGIN
    for (auto pair: items) {
        database.set(pair.first.c_str(), pair.second.c_str(), ValueSize, false);
    }
    TIMED_TEST_END("Test 1")

    std::shuffle(items.begin(), items.end(), std::default_random_engine(seed));


    // Test 2
    TIMED_TEST_BEGIN
    for (auto pair: items) {

        database.get(pair.first, buf);
    }
    TIMED_TEST_END("Test 2")


    // Test 3

    TIMED_TEST_BEGIN
    for (int i = 0; i < nrec * 5; i++) {
        if(i%100000==0)std::cout << i << std::endl;
        std::uniform_int_distribution<int> dist(0, items.size() - 1);
        auto &pair = items.at(dist(generator));
        database.get(pair.first, buf);

       /* if (i % 37 == 0) {
            intgp index = dist(generator);
            database.remove(items.at(index).first);
            items.erase(items.begin() + index);
        }*/


        if (i % 11 == 0) {
            std::string key = std::to_string(ikey++), value = generateRandomString(ValueSize);
            items.emplace_back(key, value);
            database.set(key, value.c_str(), ValueSize, false);
            database.get(key, buf);
        }

        if (i % 17 == 0) {
            int index = dist(generator);
            auto &pair = items.at(index);
            int len = pair.second.length();
            if (i % 34 == 0)len *= 2;
            std::string value = generateRandomString(len);
            database.set(pair.first, value.c_str(), len, true);
        }

    }
    TIMED_TEST_END("Test 3")


    // Test 4
    std::cout << items.size() << std::endl;
    TIMED_TEST_BEGIN
    for (auto &pair: items) {
        database.remove(pair.first);
    }
    TIMED_TEST_END("Test 4")
    std::cout <<"end" << std::endl;
    return 0;
}