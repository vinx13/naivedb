#ifndef NAIVEDB_LOCATION_H
#define NAIVEDB_LOCATION_H

namespace naivedb {


#pragma pack(1)

struct Location {
    static Location None;

    int file_no;
    int offset;

    void init() { file_no = -1; offset = 0; }

    bool isNull() const { return file_no < 0 || offset < 0; }
};

#pragma pack()


}

#endif //NAIVEDB_LOCATION_H
