#ifdef _WIN32

#include <cassert>
#include "Shlwapi.h"

#include "mmap.h"


namespace naivedb {

FD MemoryMappedFileImpl::openFile(const std::string &filename) {
    FD fd = CreateFile(
        filename.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fd == INVALID_HANDLE_VALUE) {
        // TODO
    }
    return fd;
}

void MemoryMappedFileImpl::closeFile(FD fd) {
    assert(fd != InvalidFD);
    ::closeFile(fd);
}


int MemoryMappedFileImpl::getFileSize(FD fd) {
    assert(fd >= 0);
    LARGE_INTEGER large_integer;
    if (!GetFileSizeEx(fd, &large_integer)) {
        //TODO
    }
    return static_cast<int>(large_integer.QuadPart); // FIXME: converting int64 to int32 may be not safe!
}

void MemoryMappedFileImpl::extendSize(FD fd, int size) {
    assert(fd >= 0);
    assert(size > 0);
    DWORD dwPtr = SetFilePointer(fd,
                                 size,
                                 NULL,
                                 FILE_BEGIN);

    if (dwPtr == INVALID_SET_FILE_POINTER) {
        //TODO
        return;
    }
    if (!SetEndOfFile(fd)) {
        // TODO
    }
}

bool MemoryMappedFileImpl::isExist(const std::string &filename) {
    return PathFileExists(filename.c_str()) == TRUE;
}

void *MemoryMappedFileImpl::map(FD fd, int size) {
    assert(fd != InvalidFD);
    assert(size > 0);
    map_handle_ = CreateFileMapping(fd, NULL, PAGE_READWRITE, 0, size, NULL);
    if (map_handle_ == NULL) {
        // TODO
        return nullptr;
    }
    void *view = MapViewOfFile(map_handle_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    return view;
}

void MemoryMappedFileImpl::unmap(void *view, int size) {
    assert(view);
    assert(size > 0);
    UnmapViewOfFile(view);
}


}
#endif