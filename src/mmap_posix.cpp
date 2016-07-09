#ifndef _WIN32

#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "mmap.h"

namespace medb {


int MemoryMappedFileImpl::openFile(const std::string &filename) {
    int fd = ::open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        // TODO
    }
    return fd;
}

void MemoryMappedFileImpl::closeFile(int fd) {
    assert(fd >= 0);
    close(fd);
}


int MemoryMappedFileImpl::getFileSize(int fd) {
    assert(fd >= 0);
    stat status;
    if (fstat(fd, &status) < 0) {
        //TODO
    }
    return status.st_size;
}

void MemoryMappedFileImpl::extendSize(int fd, int size) {
    assert(fd >= 0);
    assert(size > 0);
    if (lseek(fd, size - 1, SEEK_SET) < 0) {
        // TODO
    }
    if (write(fd, "", 1) < 0) {
        // TODO
    }
}

bool MemoryMappedFileImpl::isExist(const std::string &filename) {
    return access(filename.c_str(), R_OK | W_OK);
}

void *MemoryMappedFileImpl::map(int fd, int size) {
    assert(fd >= 0);
    assert(size > 0);
    void *view = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (view == MAP_FAILED) {
        // TODO
    }
    return view;
}

void MemoryMappedFileImpl::unmap(void *view, int size) {
    assert(view);
    assert(size > 0);
    munmap(view, size);
}

}

#endif
