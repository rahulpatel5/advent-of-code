#ifndef AOC8B_H
#define AOC8B_H

#include <string>
#include <sstream>

namespace aoc8b
{
    int countAddedEscapeCharacters(std::string_view line)
    {
        int total {2}; // start with 2 for double quotes at the start and end of the new string (old double quotes handled below)

        for (const auto& c : line)
        {
            if ((c == '\\') || (c == '\"'))
                total += 1;
        }
        return total;
    }

    int parseStrings(const std::string& input)
    {
        std::istringstream passage {input};
        std::string line;

        int addedEscapeLetters {0};

        while (std::getline(passage, line))
        {
            addedEscapeLetters += countAddedEscapeCharacters(line);
        }

        return addedEscapeLetters;
    }
}

#endif // AOC8B_H
