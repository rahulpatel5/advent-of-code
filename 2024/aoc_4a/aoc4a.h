#ifndef AOC4A_H
#define AOC4A_H

#include <array>
#include <string_view>

/*
find X at any point
read in each direction (horizontal, vertical or diagonal) from the X
only read where position allows reading 4 characters
check if characters spell XMAS
should avoid double counting since running from each X
*/

enum class Direction
{
    up,
    upRight,
    right,
    downRight,
    down,
    downLeft,
    left,
    upLeft,
};

namespace aoc4a
{
    constexpr std::array<int, 2> getDirection(Direction direction)
    {
        using enum Direction;

        switch (direction)
        {
        case up:        return { 0, -1};
        case upRight:   return { 1, -1};
        case right:     return { 1,  0};
        case downRight: return { 1,  1};
        case down:      return { 0,  1};
        case downLeft:  return {-1,  1};
        case left:      return {-1,  0};
        case upLeft:    return {-1, -1};
        default:        return { 0,  0}; // should not occur (raise error?)
        }
    }
    
    template <std::size_t N>
    constexpr bool checkDirection(const std::array<std::string_view, N>& arr, size_t row, size_t col, Direction direction)
    {
        std::array<int, 2> xyShifts {getDirection(direction)};
        constexpr std::string_view xmas {"XMAS"};

        for (size_t i{0}; i < xmas.size(); ++i)
        {
            size_t xShift {col + i * xyShifts.at(0)};
            size_t yShift {row + i * xyShifts.at(1)};
            if (xmas.at(i) != arr.at(yShift).at(xShift))
                return false;
        }

        return true;
    }

    template <std::size_t N>
    constexpr int countXmases(const std::array<std::string_view, N>& arr, size_t row, size_t col)
    {
        constexpr int xmasSize {4};
        int count {0};

        // can look left if on column 3 (0-indexed) or to the right
        if (col >= (xmasSize - 1))
            count += checkDirection<N>(arr, row, col, Direction::left);
        
        // can look right if on column (n - (xmasSize - 1)) or to the left, where n is final column
        if (col <= arr.at(row).size() - xmasSize)
            count += checkDirection<N>(arr, row, col, Direction::right);

        // can look at rows above if on row (xmasSize - 1) or below
        if (row >= xmasSize - 1)
        {
            count += checkDirection<N>(arr, row, col, Direction::up);

            if (col >= (xmasSize - 1))
                count += checkDirection<N>(arr, row, col, Direction::upLeft);
            
            if (col <= arr.at(row).size() - xmasSize)
                count += checkDirection<N>(arr, row, col, Direction::upRight);
        }

        // can look at rows below if on row (n - (xmasSize - 1)) or above, where n is final row
        if (row <= arr.size() - xmasSize)
        {
            count += checkDirection<N>(arr, row, col, Direction::down);

            if (col >= (xmasSize - 1))
                count += checkDirection<N>(arr, row, col, Direction::downLeft);
            
            if (col <= arr.at(row).size() - xmasSize)
                count += checkDirection<N>(arr, row, col, Direction::downRight);
        }

        return count;
    }

    template <std::size_t N>
    constexpr int parseAndCountXmas(const std::array<std::string_view, N>& arr)
    {
        int xmasCount {0};

        for (size_t i{0}; i < arr.size(); ++i)
        {
            for (size_t j{0}; j < arr.at(i).size(); ++j)
            {
                if (arr.at(i).at(j) == 'X')
                    xmasCount += countXmases<N>(arr, i, j);
            }
        }
        
        return xmasCount;
    }
}

#endif // AOC4A_H
