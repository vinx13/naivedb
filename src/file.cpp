#include "file.h"

namespace naivedb {


FileMgr::FileMgr(const std::string &prefix, int file_size) : prefix_(prefix), file_size_(file_size) {
}

FileMgr::~FileMgr() {
    closeAllFiles();
}

void FileMgr::openAllFiles(int num_files) {
    for (int i = 1; i <= num_files; i++) {
        files_.push_back(new MemoryMappedFile(getFileName(i)));
        // TODO: check existence
    }
}

void FileMgr::closeAllFiles() {
    for (MemoryMappedFile *file:files_) {
        file->close();
        delete file;
    }
    files_.clear();
}

std::string FileMgr::getFileName(int file_no) {
    return prefix_ + std::to_string(file_no);
}

FileHeader *FileMgr::getFileHeader() {
    void *p = files_[0]->get();
    assert(p);
    return reinterpret_cast<FileHeader *>(p);
}


void FileMgr::createFile() {
    int file_no = getFileHeader()->num_files;
    MemoryMappedFile *file = new MemoryMappedFile(getFileName(file_no));
    assert(!file->isExist());
    file->create(file_size_);
    files_.push_back(file);
    getFileHeader()->num_files++;
    initFile(file_no);
}

void *FileMgr::get(const Location &location) {
    assert(location.file_no >= 0 && location.file_no < files_.size());
    void *p = files_[location.file_no]->get(location.offset);
    assert(p);
    return p;
}


void FileMgr::init() {
    MemoryMappedFile *file = new MemoryMappedFile(getFileName(0));
    files_.push_back(file);
    if (file->isExist()) {
        FileHeader *header = getFileHeader();
        openAllFiles(header->num_files);
    } else {
        file->create(file_size_);
        initHeader();
    }
}


}