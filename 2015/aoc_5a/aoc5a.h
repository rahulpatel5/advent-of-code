#ifndef AOC5A_H
#define AOC5A_H

#include <iostream>
#include <sstream>

namespace aoc5a
{
    bool isVowel(char c)
    {
        return ((c == 'a') || (c == 'e') || (c == 'i') || (c == 'o') || (c == 'u'));
    }

    bool isDoubleLetter(char c, char d)
    {
        return (c == d);
    }

    bool hasDisallowedString(char c, char d)
    {
        const std::string cd {std::string() + c + d};
        return ((cd == "ab") || (cd == "cd") || (cd == "pq") || (cd == "xy"));
    }

    bool checkStringProperties(const std::string& line, int vowelTarget, int doubleTarget)
    {
        int vowelCount{0};
        int doubleCount{0};
        for (auto index{0}; index < line.size(); ++index)
        {
            vowelCount += isVowel(line[index]);
            if (index < line.size() - 1)
                doubleCount += isDoubleLetter(line[index], line[index + 1]);
            if (hasDisallowedString(line[index], line[index + 1]))
                return false;
        }
        return ((vowelCount >= vowelTarget) && (doubleCount >= doubleTarget));
    }

    int countNiceStrings(const std::string& input, int vowelTarget, int doubleTarget)
    {
        std::istringstream passage {input};
        std::string line;
        int niceCount{0};

        while (std::getline(passage, line))
        {
            niceCount += checkStringProperties(line, vowelTarget, doubleTarget);
        }
        return niceCount;
    }
}

#endif // AOC5A_H
