#include <gtest/gtest.h>
#include <fstream>
#include <random>
#include <cstring>
#include "mmap.h"

using namespace naivedb;

TEST(MMapTest, CheckExist) {
    char *filename = std::tmpnam(nullptr);
    std::ofstream os(filename);
    os.close();

    MemoryMappedFile file(filename);
    ASSERT_TRUE(file.isExist());

    std::remove(filename);
}


TEST(MMapTest, CheckNotExist) {
    char *filename = std::tmpnam(nullptr);
    MemoryMappedFile file(filename);
    ASSERT_FALSE(file.isExist());
}

TEST(MMapTest, BasicRead) {
    char *filename = std::tmpnam(nullptr);
    std::ofstream os(filename);
    char s[] = "fooFOO";
    os << s;
    os.close();

    MemoryMappedFile file(filename);
    ASSERT_NO_THROW(file.open());
    void *view = file.get();
    ASSERT_FALSE(std::memcmp(view, s, sizeof(s)));

    std::remove(filename);
}


TEST(MMapTest, BasicWrite) {
    char *filename = std::tmpnam(nullptr);
    MemoryMappedFile file(filename);

    ASSERT_NO_THROW(file.create(1024));
    int buf[] = {0x01452264, 0x01573164};
    std::memcpy(file.get(), buf, sizeof(buf));
    ASSERT_NO_THROW(file.close());

    std::ifstream is(filename);
    for (auto it = std::begin(buf); it != std::end(buf); it++) {
        int i;
        is.read(reinterpret_cast<char *>(&i), sizeof(i));
        ASSERT_EQ(i, *it);
    }

    std::remove(filename);
}

TEST(MMapTest, RandomWrite) {
    char *filename = std::tmpnam(nullptr);
    MemoryMappedFile file(filename);
    file.create(1024 * sizeof(int));

    std::uniform_int_distribution<int> dist(0, 1023);
    std::default_random_engine generator;

    int offset = dist(generator) * sizeof(int);
    void *view = file.get(offset);
    int n = dist(generator);
    memcpy(view, &n, sizeof(n));
    file.close();

    std::ifstream is(filename);
    is.seekg(offset);
    int n_read;
    is.read(reinterpret_cast<char *>(&n_read), sizeof(n_read));
    ASSERT_EQ(n, n_read);

    std::remove(filename);
}
