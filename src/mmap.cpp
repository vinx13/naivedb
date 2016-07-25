#include <cassert>

#include "mmap.h"

namespace naivedb {

int MemoryMappedFile::used_memory__ = 0, MemoryMappedFile::memory_limit__ = 1024 * 1024 * 256;
LruMap MemoryMappedFile::view_map__;
std::map<int, int> MemoryMappedFile::size_map__;

MemoryMappedFile::MemoryMappedFile(const std::string &filename) : filename_(filename), fd_(InvalidFD) {
    impl_ = new MemoryMappedFileImpl();
}

MemoryMappedFile::~MemoryMappedFile() {
    if (fd_ != InvalidFD) {
        close();
    }
    delete impl_;
}

void MemoryMappedFile::open() {
    fd_ = impl_->openFile(filename_);
    assert(size_map__.find(fd_) == size_map__.end());
    size_map__[fd_] = impl_->getFileSize(fd_);
}

void *MemoryMappedFile::get(int offset) {
    assert(offset >= 0);
    if (fd_ == InvalidFD) open();
    void *view = view_map__.get(fd_);
    if (view == nullptr) {
        view = map();
    }
    return reinterpret_cast<void *>(reinterpret_cast<char *>(view) + offset);
}


bool MemoryMappedFile::isExist() {
    return impl_->isExist(filename_);
}

void MemoryMappedFile::create(int size) {
    fd_ = impl_->openFile(filename_);
    impl_->extendSize(fd_, size);
    assert(size_map__.find(fd_) == size_map__.end());
    size_map__[fd_] = size;
}


void MemoryMappedFile::close() {
    impl_->closeFile(fd_);
    void *view = view_map__.get(fd_);
    if (view) {
        int size = size_map__.at(fd_);
        assert(size > 0);
        impl_->unmap(view, size);
        view_map__.erase(fd_);
        used_memory__ -= size;
    }
    size_map__.erase(fd_);
    fd_ = InvalidFD;
}

void *MemoryMappedFile::map() {
    assert(fd_ != InvalidFD);
    int size = size_map__[fd_];
    assert(size > 0);
    while (used_memory__ + size > memory_limit__) {
        auto pair = view_map__.removeLru();
        int size = size_map__[pair.first];
        assert(size > 0);
        impl_->unmap(pair.second, size);
        used_memory__ -= size;
    }
    used_memory__ += size;
    void *view = impl_->map(fd_, size);
    view_map__.put(fd_, view);
    return view;
}


}

