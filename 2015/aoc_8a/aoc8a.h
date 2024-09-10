#ifndef AOC8A_H
#define AOC8A_H

#include <string>
#include <sstream>
#include <iostream>

namespace aoc8a
{
    bool isEscapeSequence(std::string_view line, auto index)
    {
        // hard-coding character escape sequence of \\ or \"
        return ((line[index] == '\\') && ((line[index + 1] == '\\') || (line[index + 1] == '\"')));
    }

    bool isHexadecimal(char c)
    {
        return (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')));
    }

    bool isHexadecimalEscape(std::string_view line, auto index)
    {
        if (index >= line.size() - 3)
            return false;
        // catch sequences e.g. a\\x12, which aren't hexadecimal
        if ((index >= 2) && (line[index - 1] == '\\') && (line[index - 2] != '\\'))
            return false;
        
        return ((line[index] == '\\') && (line[index + 1] == 'x') && isHexadecimal(line[index + 2]) && isHexadecimal(line[index + 3]));
    }

    bool isEscapePair(std::string_view line, auto index)
    {
        if (index >= line.size() - 3)
            return false;
        
        // avoid double-counting sequences of \\\\ or \\\"
        // this doesn't work for longer sequences, e.g. \\\\\"
        return ((line[index] == '\\') && (line[index + 1] == '\\') && (isHexadecimalEscape(line, index + 2) || isEscapeSequence(line, index + 2)));
    }

    int countEscapeCharacters(std::string_view line)
    {
        int subtractCount {2}; // start with 2 for double quotes at the start and end of the string

        for (auto index{1}; index < (line.size() - 2); ++index)
        {
            if ((line[index] != '\\') || isEscapePair(line, index))
                continue;
            
            if (isHexadecimalEscape(line, index))
                subtractCount += 3;
            else if (isEscapeSequence(line, index))
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
