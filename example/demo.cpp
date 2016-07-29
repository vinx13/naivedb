#include <iostream>
#include <string>
#include <cstdlib>
#include "database.h"

using namespace naivedb;

void setUp(const std::string &name) {
    Database db(name);
    const int MaxN = 100000;
    for(int i = 0; i < MaxN; i++){
        db.set<int>(std::to_string(i), i, false);
    }
}

int main() {
    const std::string name = std::tmpnam(nullptr);

    setUp(name);

    Database db(name);

    while(true) {
        std::cout << "Enter key to query: ";
        std::string key;
        std::cin >> key;
        try {
            int value = db.get<int>(key);
            std::cout << "Value: " << value << std::endl;
        } catch(NotFoundException &e){
            std::cout << e.what() << std::endl;
        }
    }
    return 0;
}