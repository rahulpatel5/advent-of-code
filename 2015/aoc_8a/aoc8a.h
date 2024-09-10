#ifndef AOC8A_H
#define AOC8A_H

#include "escape.h"
#include <string>
#include <sstream>
#include <iostream>

namespace aoc8a
{
    int countEscapeCharacters(std::string_view line)
    {
        int subtractCount {2}; // start with 2 for double quotes at the start and end of the string

        for (auto index{1}; index < (line.size() - 2); ++index)
        {
            if ((line[index] != '\\') || escape::isEscapePair(line, index))
                continue;
            
            if (escape::isHexadecimalEscape(line, index))
                subtractCount += 3;
            else if (escape::isEscapeSequence(line, index))
                subtractCount += 1;
        }
        return subtractCount;
    }

    int parseStrings(const std::string& input)
    {
        std::istringstream passage {input};
        std::string line;

        int countEscapeLetters {0};

        while (std::getline(passage, line))
        {
            countEscapeLetters += countEscapeCharacters(line);
        }

        return countEscapeLetters;
    }
}

#endif // AOC8A_H
