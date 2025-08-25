#ifndef AOC4A_H
#define AOC4A_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <algorithm>

/*
FOR each card:
    READ winning numbers and scratch card numbers
    COUNT each winning number in scratch card section
    COUNT 1 point for first match, and double for each after that
    ADD card total to running total

RETURN running total
*/

namespace aoc4a
{
    using Points = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Number = int;
    using Numbers = std::vector<Number>;

    Numbers getNumbers(const std::string& line)
    {
        Numbers numbers {};
        size_t space {line.find(' ')};
        size_t index {space + 1};
        while (index != std::string::npos)
        {
            size_t nextSpace {line.find(' ', index + 1)};
            std::string numberStr {};
            if (nextSpace != std::string::npos)
                numberStr = line.substr(index, nextSpace - index);
            else
                numberStr = line.substr(index);
            if (numberStr.size() > 0)
                numbers.push_back(std::stoi(numberStr));

            if (nextSpace != std::string::npos)
                index = nextSpace + 1;
            else
                index = nextSpace;
        }
        return numbers;
    }

    Points countWins(const Numbers& card, const Numbers& winning)
    {
        Points points {0};
        for (Number num : card)
        {
            if (std::find(winning.begin(), winning.end(), num) != winning.end())
            {
                if (points == 0)
                    points = 1;
                else
                    points *= 2;
            }
        }
        return points;
    }

    template <std::size_t L>
    Points getTotalWinningPoints(const Lines<L>& lines)
    {
        Points total {0};

        for (std::string_view line : lines)
        {
            std::string fullLine {std::string(line)};
            size_t colon {fullLine.find(':')};
            size_t pipe {fullLine.find('|')};

            Numbers winningNumbers {getNumbers(fullLine.substr(colon + 1, pipe - (colon + 1)))};
            Numbers scratchcard {getNumbers(fullLine.substr(pipe + 1))};
            total += countWins(scratchcard, winningNumbers);
        }

        return total;
    }
}

#endif // AOC4A_H
