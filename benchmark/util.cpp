#include <cstdlib>
#include <algorithm>
#include <string>
#include <iterator>

#include "util.h"

std::string generateRandomString(size_t length)
{
    const char* charmap = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t charmapLength = strlen(charmap);
    auto generator = [&](){ return charmap[std::rand()%charmapLength]; };
    std::string result;
    result.reserve(length);
    std::generate_n(std::back_inserter(result), length, generator);
    return result;
}