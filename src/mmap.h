#ifndef NAIVEDB_MMAP_H
#define NAIVEDB_MMAP_H

#include <memory>
#include <map>
#include <string>

#include "platform.h"
#include "lrumap.h"

namespace naivedb {



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
    FD fd_;
    MemoryMappedFileImpl *impl_;

    void * map();
};


class MemoryMappedFileImpl {
    friend class MemoryMappedFile;

private:

    void create(int size);

    void extendSize(FD fd, int size);

    FD openFile(const std::string &filename);

    void closeFile(FD fd);

    int getFileSize(FD fd);

    void *map(FD fd, int size);

    void unmap(void *view, FD size);

    bool isExist(const std::string &filename);

#ifdef WIN32_
    HANDLE map_handle_ = INVALID_HANDLE_VALUE;
#endif

};


}

#endif //NAIVEDB_MMAP_H
