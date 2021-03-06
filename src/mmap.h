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

    static void setMemoryLimitation(int memory_limitation) { MemoryMappedFile::memory_limitation__ = memory_limitation; }

    MemoryMappedFile(const std::string &filename);

    ~MemoryMappedFile();

    void *get(int offset = 0);

    bool isExist();

    void create(int size);

    void close();

private:
    void open();

    static int used_memory__, memory_limitation__;
    static LruMap<FD, void *> view_map__;
    static std::map<FD, int> size_map__;
    std::string filename_;
    FD fd_;
    MemoryMappedFileImpl *impl_;

    void *map();
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

    void unmap(void *view, int size);

    bool isExist(const std::string &filename);

#ifdef _WIN32
    HANDLE map_handle_ = INVALID_HANDLE_VALUE;
#endif

};


}

#endif //NAIVEDB_MMAP_H
