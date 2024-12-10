#ifndef AOC1B_H
#define AOC1B_H

#include <array>
#include <string>
#include <algorithm>

#include <iostream>

/*
parse input into two arrays

get similarity score
*/

namespace aoc1b
{
    template <typename T>
    int countNumberInArray(const T& array, int number)
    {
        int count {0};
        for (const auto& element : array)
        {
            if (element == number)
                ++count;
        }
        return count * number;
    }

    template <typename T>
    int getSimilarityScore(const T& array1, const T& array2)
    {
        int similarityScore {0};
        for (auto i{0}; i < array1.size(); ++i)
        {
            int number {array1[i]};
            similarityScore += countNumberInArray(array2, number);
        }
        return similarityScore;
    }

    template <typename T, int arraySize>
    int parseAndGetSimilarityScore(const T& lines)
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

        return getSimilarityScore(array1, array2);
    }
}

#endif // AOC1B_H
