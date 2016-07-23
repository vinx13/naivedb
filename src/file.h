#ifndef NAIVEDB_FILE_H
#define NAIVEDB_FILE_H

#include <cassert>
#include <vector>
#include "mmap.h"
#include "location.h"
#include "storage.h"
#include "datafile.h"

namespace naivedb {


class DBStore;


class FileMgr {
public:
    FileMgr(const std::string &prefix, int file_size, DBStore *db_store);

    virtual ~FileMgr() {
        closeAllFiles();
    }

    void openAllFiles(int num_files);

    void closeAllFiles();

protected:

    virtual void initFile(int file_no) = 0;

    const std::string prefix_;
    DBStore *db_store_;
    std::vector<MemoryMappedFile *> files_;
    int file_size_;

    void createFile();

    void *get(const Location &location);

    std::string getFileName(int file_no);


    FileHeader *getFileHeader();

};


}

#endif //NAIVEDB_FILE_H
