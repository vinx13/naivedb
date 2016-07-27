#include <cstdlib>
#include <algorithm>
#include <string>
#include <iterator>
#include <cstring>

#include "util.h"

std::string generateRandomString(size_t length)
{
    const char* charmap = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t charmap_length = std::strlen(charmap);
    auto generator = [&](){ return charmap[std::rand() % charmap_length]; };
    std::string result;
    result.reserve(length);
    std::generate_n(std::back_inserter(result), length, generator);
    return result;
}