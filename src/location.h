#ifndef NAIVEDB_LOCATION_H
#define NAIVEDB_LOCATION_H

namespace naivedb {


#pragma pack(1)

struct Location {
    int file_no;
    int offset;

    Location(int file_no = -1, int offset = 0) : file_no(file_no), offset(offset) { }

    void init() { file_no = -1; }

    bool isNull() const { return file_no < 0 || offset < 0; }
};

#pragma pack()


}

#endif //NAIVEDB_LOCATION_H
