#ifndef AOC1A_H
#define AOC1A_H

#include <array>
#include <string>
#include <algorithm>

#include <iostream>

/*
parse input into two arrays

sort arrays in ascending order

get difference and total of differences
*/

namespace aoc1a
{
    int absoluteDifference(int a, int b)
    {
        return ((a - b) < 0) ? b - a : a - b;
    }

    template <typename T>
    int getTotalDifference(const T& array1, const T& array2)
    {
        int totalDifference {0};
        for (auto i{0}; i < array1.size(); ++i)
        {
            int difference {absoluteDifference(array1[i], array2[i])};
            totalDifference += difference;
        }
        return totalDifference;
    }

    template <typename T, int arraySize>
    int parseAndGetTotalDifference(const T& lines)
    {
        std::array<int, arraySize> array1 {};
        std::array<int, arraySize> array2 {};

        // parse input and extract numbers
        for (auto i{0}; i < lines.size(); ++i)
        {
            size_t spaces {lines[i].find_first_of("   ")};
            array1[i] = std::atoi(std::string(lines[i].substr(0, spaces)).c_str());
            array2[i] = std::atoi(std::string(lines[i].substr(spaces + 3)).c_str());
        }

        // sort arrays in ascending order
        std::sort(array1.begin(), array1.end());
        std::sort(array2.begin(), array2.end());

        return getTotalDifference(array1, array2);
    }
}

#endif // AOC1A_H
