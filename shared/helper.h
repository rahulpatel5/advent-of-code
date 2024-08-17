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

    int minThreeNumbers(int a, int b, int c)
    {
        int interim{ (a < b) ? a : b };
        return (interim < c) ? interim : c;
    }
}

#endif
