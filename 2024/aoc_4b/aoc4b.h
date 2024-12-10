#ifndef AOC4B_H
#define AOC4B_H

#include <array>
#include <string_view>

/*
find A at any point
read in diagonals from the A
only read where position allows reading each corner
check if characters spell MAS on each diagonal
should avoid double counting since running from each A
*/

namespace aoc4b
{
    template <std::size_t N>
    constexpr bool checkCorners(const std::array<std::string_view, N>& arr, size_t row, size_t col)
    {
        bool topLeftM {arr.at(row - 1).at(col - 1) == 'M'};
        bool topLeftS {arr.at(row - 1).at(col - 1) == 'S'};
        bool topRightM {arr.at(row - 1).at(col + 1) == 'M'};
        bool topRightS {arr.at(row - 1).at(col + 1) == 'S'};
        bool bottomLeftM {arr.at(row + 1).at(col - 1) == 'M'};
        bool bottomLeftS {arr.at(row + 1).at(col - 1) == 'S'};
        bool bottomRightM {arr.at(row + 1).at(col + 1) == 'M'};
        bool bottomRightS {arr.at(row + 1).at(col + 1) == 'S'};

        bool isDiagonal1MS {topLeftM && bottomRightS};
        bool isDiagonal1SM {topLeftS && bottomRightM};
        bool isDiagonal2MS {bottomLeftM && topRightS};
        bool isDiagonal2SM {bottomLeftS && topRightM};

        return ((isDiagonal1MS || isDiagonal1SM) && (isDiagonal2MS || isDiagonal2SM));
    }

    template <std::size_t N>
    constexpr int countMases(const std::array<std::string_view, N>& arr, size_t row, size_t col)
    {
        int count {0};

        // 'A' is valid if space on top-left, top-right, bottom-left and bottom-right
        if ((col >= 1) && (col <= arr.at(row).size() - 2) && (row >= 1) && (row <= arr.size() - 2))
            count += checkCorners<N>(arr, row, col);
        
        return count;
    }

    template <std::size_t N>
    constexpr int parseAndCountMas(const std::array<std::string_view, N>& arr)
    {
        int xmasCount {0};

        for (size_t i{0}; i < arr.size(); ++i)
        {
            for (size_t j{0}; j < arr.at(i).size(); ++j)
            {
                if (arr.at(i).at(j) == 'A')
                    xmasCount += countMases<N>(arr, i, j);
            }
        }
        
        return xmasCount;
    }
}

#endif // AOC4B_H
