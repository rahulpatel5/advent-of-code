#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <algorithm>

namespace helper
{
    auto getNumberOfChars(std::string_view input, char character)
    {
        return std::ranges::count(input, character);
    }
}

#endif
