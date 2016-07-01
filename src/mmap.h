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

// represent disk files that are mapped into memory
class MemoryMapper {
public:
    static std::shared_ptr<MemoryMapper> getInstance();

    // create or open a file and map it to memory, if len is 0, default file size is used
    void *open(const std::string &filename, int len = 0);

    // extend the mapped file, use open if the file hasn't been openned
    void *grow(const std::string &filename, int new_len);

private:
    MemoryMapper();

    void closeAll();

    std::shared_ptr<MemoryMapper> instance__;
};

}

#endif //MEDB_MMAP_H
