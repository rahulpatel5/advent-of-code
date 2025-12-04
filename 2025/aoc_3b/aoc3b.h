#ifndef AOC3B_H
#define AOC3B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <ranges>

/*
FOR each line:
    FIND highest number in positions in first (n - 12) positions
    (where n is the length of the line)
    FOR each of the highest number:
        FIND the largest number that can be formed
        ADD the largest of all of these numbers to the sum

RETURN sum
*/

namespace aoc3b
{
    using JoltInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using NumberStr = std::string;
    using Position = std::size_t;

    const int maxDigits { 12 };

    Position getPositionInNumberString(const NumberStr& numberString, char target)
    {
        return numberString.find(target);
    }

    JoltInt getLargestJoltage(const NumberStr& row)
    {
        NumberStr largest {};

        Position currentPos {0};
        for (Position maxPos{row.size() - maxDigits}; maxPos < row.size(); ++maxPos)
        {
            NumberStr numberStr { row.substr(currentPos, (maxPos + 1) - currentPos) };
            for (char c{'9'}; c >= '0'; --c)
            {
                Position pos { getPositionInNumberString(numberStr, c) };
                if (pos != -1)
                {
                    largest += numberStr.at(pos);
                    currentPos += pos + 1;
                    break;
                }
            }
        }

        return std::stoll(largest);
    }

    template <std::size_t L>
    JoltInt getTotalJoltage(const Lines<L>& lines)
    {
        JoltInt sum {0};

        for (std::string_view line : lines)
        {
            NumberStr row { std::string(line) };
            sum += getLargestJoltage(row);
        }

        return sum;
    }
}

#endif // AOC3B_H
