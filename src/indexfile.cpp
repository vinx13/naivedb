#include <cassert>

#include "indexfile.h"
#include "dbstore.h"

namespace naivedb {


IndexFileMgr::IndexFileMgr(const std::string &filename) : FileMgr(filename, DefaultIndexFileSize) {
}

IndexFileHeader *IndexFileMgr::getIndexHeader() {
    return reinterpret_cast<IndexFileHeader *>(FileMgr::getFileHeader());
}


IndexRecord *IndexFileMgr::recordAt(const Location &location) {
    assert(!location.isNull());
    return reinterpret_cast<IndexRecord *>(get(location));
}

Location IndexFileMgr::alloc() {
    IndexFileHeader *header = getIndexHeader();
    if (header->empty_head.isNull()) {
        createFile();
    }
    Location loc = header->empty_head;
    header->empty_head = recordAt(loc)->next;
    return loc;
}

void IndexFileMgr::collect(const Location &location) {
    addToHead(location);
}

void IndexFileMgr::initFile(int file_no) {
    IndexRecord *cur = reinterpret_cast<IndexRecord *>(get({file_no, 0}));

    // void *end = reinterpret_cast<void *>(reinterpret_cast<char *>(cur) + DefaultIndexFileSize);
    int cur_ofs = 0;
    const int record_size = sizeof(IndexRecord), size = DefaultIndexFileSize;
    while (true) {
        int next_ofs = cur_ofs + record_size;
        if (next_ofs + record_size < size) {
            cur->next = {file_no, next_ofs};
            ++cur;
            cur_ofs += record_size;

        } else {
            //cur->next = getIndexHeader()->empty_head;
            addToHead({file_no, cur_ofs});
            getIndexHeader()->empty_head = {file_no, 0};
            break;
        }
    }
}

void IndexFileMgr::addToHead(const Location &location) {
    assert(!location.isNull());
    IndexFileHeader *header = getIndexHeader();
    recordAt(location)->next = header->empty_head;
    header->empty_head = location;
}


void IndexFileMgr::initHeader() {
    IndexFileHeader *header = getIndexHeader();
    header->num_files = 1;
    header->empty_head.init();
    header->first_leaf.init();
    header->tree_root.init();
}


}