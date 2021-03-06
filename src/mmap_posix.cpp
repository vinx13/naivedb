#ifndef _WIN32

#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "mmap.h"
#include "exception.h"

namespace naivedb {


int MemoryMappedFileImpl::openFile(const std::string &filename) {
    FD fd = ::open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        throw FileIOException();
    }
    return fd;
}

void MemoryMappedFileImpl::closeFile(FD fd) {
    assert(fd >= 0);
    close(fd);
}


int MemoryMappedFileImpl::getFileSize(FD fd) {
    assert(fd >= 0);
    struct stat status;
    if (fstat(fd, &status) < 0) {
        throw FileIOException();
    }
    return status.st_size;
}

void MemoryMappedFileImpl::extendSize(FD fd, int size) {
    assert(fd >= 0);
    assert(size > 0);
    if (lseek(fd, size - 1, SEEK_SET) < 0 || write(fd, "", 1) < 0) {
        throw FileIOException();
    }
}

bool MemoryMappedFileImpl::isExist(const std::string &filename) {
    return access(filename.c_str(), R_OK | W_OK) == 0;
}

void *MemoryMappedFileImpl::map(FD fd, int size) {
    assert(fd >= 0);
    assert(size > 0);
    void *view = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (view == MAP_FAILED) {
        throw FileIOException();
    }
    return view;
}

void MemoryMappedFileImpl::unmap(void *view, int size) {
    assert(view);
    assert(size > 0);
    if (0 != munmap(view, size)) {
        throw FileIOException();
    }
}

}

#endif
