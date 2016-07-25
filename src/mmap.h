#ifndef NAIVEDB_MMAP_H
#define NAIVEDB_MMAP_H

#include <memory>
#include <map>
#include <string>

#include "lrumap.h"

namespace naivedb {

#ifdef _WIN32
#include <windows.h>
typedef HANDLE FD;
#else
typedef int FD;
#endif

class MemoryMappedFileImpl;

class MemoryMappedFile {
public:
    MemoryMappedFile(const std::string &filename);

    ~MemoryMappedFile();

    void *get(int offset = 0);

    bool isExist();

    void create(int size);

    void close();

private:
    void open();

    static int used_memory__, memory_limit__;
    static LruMap view_map__;
    static std::map<int, int> size_map__;
    std::string filename_;
    int fd_;
    MemoryMappedFileImpl *impl_;

    void * map();
};


class MemoryMappedFileImpl {
    friend class MemoryMappedFile;

private:

    void create(int size);

    void extendSize(int fd, int size);

    int openFile(const std::string &filename);

    void closeFile(int fd);

    int getFileSize(int fd);

    void *map(int fd, int size);

    void unmap(void *view, int size);

    bool isExist(const std::string &filename);
};


}

#endif //NAIVEDB_MMAP_H
