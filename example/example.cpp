#include <string>
#include "database.h"

using namespace naivedb;

int main() {

    // create or open database
    const std::string database_name = "database";
    DatabaseOption option;
    option.memory_limitation = 1024 * 1024 * 1024; // set max memory to 1GB
    Database *db = new Database(database_name, option);

    // write
    int value = 1;
    db->set("1", (void *)&value, sizeof(value), false /* overwrite not allowed */ );
    db->set<int>("2", 2, false); // this is helper function for POD type

    // read
    int length = db->get("1", (void *)&value); // length == sizeof(int)
    db->get<int>("2"); // return 2

    // remove
    db->remove("1");

    // close database
    delete db;

    return 0;
}  