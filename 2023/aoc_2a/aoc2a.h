#ifndef AOC2A_H
#define AOC2A_H

#include <array>
#include <string_view>
#include <map>
#include <string>

/*
READ main bag

FOR each game:
    COMPARE each subset of cubes
    IF any subset is greater than the number in the main bag:
        FAIL and skip to next game
    ELSE:
        CONTINUE
    IF all subsets pass:
        ADD game ID to passed IDs

RETURN sum of passed IDs
*/

namespace aoc2a
{
    using IdInt = long long;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    std::map<std::string, int> bag {
        { "red",   12 },
        { "green", 13 },
        { "blue",  14 }
    };

    IdInt addIDIfGameIsPossible(const std::string& line)
    {
        size_t space {line.find(' ')};
        size_t colon {line.find(':')};
        std::string gameID {line.substr(space + 1, colon - (space + 1))};

        size_t index {colon};
        while (index != std::string::npos)
        {
            size_t firstSpace {line.find(' ', index)};
            size_t secondSpace {line.find(' ', firstSpace + 1)};

            size_t comma {line.find(',', secondSpace)};
            size_t semiColon {line.find(';', secondSpace)};
            size_t nextPunct { (comma < semiColon) ? comma : semiColon };

            IdInt value {std::stoll(line.substr(firstSpace + 1, secondSpace - (firstSpace + 1)))};
            std::string colour {line.substr(secondSpace + 1, nextPunct - (secondSpace + 1))};

            if (value > bag.at(colour))
                return 0;
            
            index = nextPunct;
        }

        return std::stoll(gameID);
    }

    template <std::size_t L>
    IdInt getSumOfIDs(const Lines<L>& lines)
    {
        IdInt sumPossibleGames {0};

        for (std::string_view line : lines)
        {
            sumPossibleGames += addIDIfGameIsPossible(std::string(line));
        }

        return sumPossibleGames;
    }
}

#endif // AOC2A_H
