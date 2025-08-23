#ifndef AOC2B_H
#define AOC2B_H

#include <array>
#include <string_view>
#include <string>
#include <stdexcept>

/*
FOR each game:
    LOOK AT each group of subset of cubes
    TRACK max number of each colour in the game
    MULTIPLY these max numbers to get the 'power'
    ADD the power to the sum of powers

RETURN sum of powers
*/

namespace aoc2b
{
    using IdInt = long long;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    IdInt getPowerOfGame(const std::string& line)
    {
        // assuming no funny business with 0 of one colour
        IdInt blue  {1};
        IdInt green {1};
        IdInt red   {1};

        size_t index {line.find(':')};
        while (index != std::string::npos)
        {
            size_t firstSpace {line.find(' ', index)};
            size_t secondSpace {line.find(' ', firstSpace + 1)};

            size_t comma {line.find(',', secondSpace)};
            size_t semiColon {line.find(';', secondSpace)};
            size_t nextPunct { (comma < semiColon) ? comma : semiColon };

            IdInt value {std::stoll(line.substr(firstSpace + 1, secondSpace - (firstSpace + 1)))};
            std::string colour {line.substr(secondSpace + 1, nextPunct - (secondSpace + 1))};

            if (colour == "blue" && value > blue)
                blue = value;
            else if (colour == "green" && value > green)
                green = value;
            else if (colour == "red" && value > red)
                red = value;

            index = nextPunct;
        }

        return blue * green * red;
    }

    template <std::size_t L>
    IdInt getSumOfPowers(const Lines<L>& lines)
    {
        IdInt sumPossibleGames {0};

        for (std::string_view line : lines)
        {
            sumPossibleGames += getPowerOfGame(std::string(line));
        }

        return sumPossibleGames;
    }
}

#endif // AOC2B_H
