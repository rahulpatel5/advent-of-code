#ifndef AOC3B_H
#define AOC3B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <cassert>

/*
FIND each gear in the schematic

FOR each gear in the schematic:
    IDENTIFY each number adjacent to the gear
    IF two adjacent numbers (diagonal allowed):
        MULTIPLY the two numbers
        ADD this product to the sum of part numbers
    ELSE:
        SKIP to next gear

RETURN sum of part numbers
*/

namespace aoc3b
{
    using GearInt = long long;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;
    using DigitChain = std::vector<int>;
    using Tile = std::pair<size_t, size_t>;
    using DigitTiles = std::vector<Tile>;

    using Gear = std::pair<size_t, size_t>;
    using Gears = std::vector<Gear>;

    template <std::size_t N>
    bool isDigit(const Lines<N>& map, size_t row, size_t col)
    {
        return (map.at(row).at(col) >= '0' && map.at(row).at(col) <= '9');
    }

    template <std::size_t N>
    Gears getGears(const Lines<N>& map)
    {
        Gears gears {};
        for (size_t row{0}; row < map.size(); ++row)
        {
            for (size_t col{0}; col < map.at(row).size(); ++col)
            {
                if (map.at(row).at(col) == '*')
                    gears.push_back(std::make_pair(row, col));
            }
        }
        return gears;
    }

    template <std::size_t N>
    bool isAnyPartNumberAboveOrBelow(const Lines<N>& map, size_t row, size_t col)
    {
        return ((col > 0 && isDigit(map, row, col - 1)) || isDigit(map, row, col) || ((col < map.at(row).size() - 1) && isDigit(map, row, col + 1)));
    }

    template <std::size_t N>
    bool isSecondPartNumberAboveOrBelow(const Lines<N>& map, size_t row, size_t col)
    {
        return (col > 0 && col < map.at(row).size() - 1 && isDigit(map, row, col - 1) && !isDigit(map, row, col) & isDigit(map, row, col + 1));
    }

    template <std::size_t N>
    bool areTwoPartNumbers(const Lines<N>& map, Gear gear)
    {
        int partNumbers {0};
        size_t row {gear.first};
        size_t col {gear.second};

        // check above gear
        if (row > 0)
        {
            if (isAnyPartNumberAboveOrBelow<N>(map, row - 1, col))
            {
                ++partNumbers;

                // we know there are two part numbers if there's a gap
                if (isSecondPartNumberAboveOrBelow<N>(map, row - 1, col))
                    ++partNumbers;
            }
        }

        // check to left of gear
        if (col > 0 && isDigit(map, row, col - 1))
            ++partNumbers;
        
        // check to right of gear
        if (col < map.at(row).size() - 1 && isDigit(map, row, col + 1))
            ++partNumbers;
        
        // check below gear
        if (row < map.size() - 1)
        {
            if (isAnyPartNumberAboveOrBelow<N>(map, row + 1, col))
            {
                ++partNumbers;

                // we know there are two part numbers if there's a gap
                if (isSecondPartNumberAboveOrBelow<N>(map, row + 1, col))
                    ++partNumbers;
            }
        }

        return partNumbers == 2;
    }

    template <std::size_t N>
    int getNumber(const Lines<N>& map, size_t row, size_t col)
    {
        // go backwards to find first digit
        size_t start {col};
        // unless we're on the left side of the schematic
        while (start > 0 && isDigit<N>(map, row, start - 1))
        {
            --start;
        }

        std::string digitStr {""};
        while (start < map.at(row).size() && isDigit<N>(map, row, start))
        {
            digitStr += map.at(row).at(start);
            ++start;
        }

        return std::stoll(digitStr);
    }

    template <std::size_t N>
    GearInt getProductOfTwoPartNumbers(const Lines<N>& map, size_t row, size_t col)
    {
        DigitTiles digits {};

        // check left and right
        if (col > 0 && isDigit<N>(map, row, col - 1))
            digits.push_back(std::make_pair(row, col - 1));
        if (col < map.at(row).size() - 1 && isDigit<N>(map, row, col + 1))
            digits.push_back(std::make_pair(row, col + 1));
        
        // check top left
        if (row > 0 && col > 0 && isDigit<N>(map, row - 1, col - 1))
            digits.push_back(std::make_pair(row - 1, col - 1));
        // check top middle, if none in top left
        if (row > 0 && col > 0 && !isDigit<N>(map, row - 1, col - 1) && isDigit<N>(map, row - 1, col))
            digits.push_back(std::make_pair(row - 1, col));
        // check top right, if gap in top middle
        if (row > 0 && col < map.at(row).size() - 1 && !isDigit<N>(map, row - 1, col) && isDigit<N>(map, row - 1, col + 1))
            digits.push_back(std::make_pair(row - 1, col + 1));
        
        // check bottom left
        if (row < map.size() - 1 && col > 0 && isDigit<N>(map, row + 1, col - 1))
            digits.push_back(std::make_pair(row + 1, col - 1));
        // check bottom middle, if none in bottom left
        if (row < map.size() - 1 && col > 0 && !isDigit<N>(map, row + 1, col - 1) && isDigit<N>(map, row + 1, col))
            digits.push_back(std::make_pair(row + 1, col));
        // check bottom right, if gap in bottom middle
        if (row < map.size() - 1 && col < map.at(row).size() - 1 && !isDigit<N>(map, row + 1, col) && isDigit<N>(map, row + 1, col + 1))
            digits.push_back(std::make_pair(row + 1, col + 1));

        // assert that we have two part numbers
        assert(digits.size() == 2 && "Expected to have only two part numbers.\n");

        int first {getNumber<N>(map, digits.at(0).first, digits.at(0).second)};
        int second {getNumber<N>(map, digits.at(1).first, digits.at(1).second)};

        return first * second;
    }

    template <std::size_t N>
    GearInt addIfGearNumber(const Lines<N>& map, Gear gear)
    {
        if (!areTwoPartNumbers<N>(map, gear))
            return 0;
        
        // we know there are only 2 part numbers, so get these
        return getProductOfTwoPartNumbers<N>(map, gear.first, gear.second);
    }

    template <std::size_t L>
    GearInt getSumOfGearNumbers(const Lines<L>& lines)
    {
        Gears gears {getGears<L>(lines)};

        GearInt sumGearNum {0};

        for (Gear gear : gears)
        {
            sumGearNum += addIfGearNumber<L>(lines, gear);
        }

        return sumGearNum;
    }
}

#endif // AOC3B_H
