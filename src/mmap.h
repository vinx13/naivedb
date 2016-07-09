#ifndef MEDB_MMAP_H
#define MEDB_MMAP_H

#include <memory>
#include <map>
#include <string>
#include <tuple>

namespace medb {

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

    void open();

    void *get();

    void grow(int new_size);

    bool isExist();

    void create(int size);

    void close();

private:
    std::string filename_;
    void *view_;
    int size_;
    int fd_;
    MemoryMappedFileImpl *impl_;
};


class MemoryMappedFileImpl{
    friend class MemoryMappedFile;
private:
    void extendSize(int fd, int size);

    int openFile(const std::string &filename);

    void closeFile(int fd);

    int getFileSize(int fd);

    void *map(int fd, int size);

    void unmap(void *view, int size);

    bool isExist(const std::string &filename);
};

}

#endif //MEDB_MMAP_H
