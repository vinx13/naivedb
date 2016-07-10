#include <cassert>
#include "mmap.h"

namespace medb {


MemoryMappedFile::MemoryMappedFile(const std::string &filename) : filename_(filename), size_(0), fd_(-1) ,view_(nullptr){
    impl_ = new MemoryMappedFileImpl();
}

MemoryMappedFile::~MemoryMappedFile() {
    if (view_) {
        impl_->closeFile(fd_);
        impl_->unmap(view_, size_);
    }
    delete impl_;
}

void *MemoryMappedFile::get(int offset) {
    assert(view_);
    assert(offset >= 0);
    return static_cast<void*>(static_cast<char *>(view_)+offset);
}


bool MemoryMappedFile::isExist() {
    return impl_->isExist(filename_);

}

void MemoryMappedFile::open() {
    fd_ = impl_->openFile(filename_);
    size_ = impl_->getFileSize(fd_);
    view_ = impl_->map(fd_, size_);
}

void MemoryMappedFile::grow(int new_size) {
    if (view_) {
        impl_->unmap(view_, size_);
    } else {
        open();
    }
    impl_->extendSize(fd_, new_size);
    size_ = new_size;
    impl_->map(fd_, size_);
}


void MemoryMappedFile::create(int size) {
    fd_ = impl_->openFile(filename_);
    impl_->extendSize(fd_, size);
    size_ = size;
    view_ = impl_->map(fd_, size_);
}


void MemoryMappedFile::close() {
    impl_->unmap(view_, size_);
    impl_->closeFile(fd_);
    fd_ = -1;
    size_ = 0;
    view_ = nullptr;
}



}

