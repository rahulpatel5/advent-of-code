#ifndef AOC3B_H
#define AOC3B_H

#include <string>
#include <sstream>
#include <vector>

namespace aoc3b
{
    template <typename T>
    int multiply(const T& function)
    {
        std::size_t openBracket {function.find_first_of('(')};
        std::size_t comma {function.find_first_of(',')};
        std::size_t closeBracket {function.find_first_of(')')};

        if (comma > 7)
            return 0;

        int firstDigit {std::stoi(function.substr(openBracket + 1, comma - openBracket - 1))};
        int secondDigit {std::stoi(function.substr(comma + 1, closeBracket - comma - 1))};

        return firstDigit * secondDigit;
    }

    template <typename T>
    int parseLineAndMultiply(const T& line)
    {
        size_t start {0};
        size_t end {0};
        int product {0};

        while ((end = line.find("mul(", start)) != std::string_view::npos)
        {
            size_t nextBracket {line.find_first_of(')', end)};
            if (nextBracket - end <= 11)
                product += multiply(line.substr(end, nextBracket));
            start = end + 1;
        }

        return product;
    }

    template <typename T>
    std::vector<std::string> getEnabledInstructions(const T& line)
    {
        std::vector<std::string> enabled {};
        size_t start {0};
        size_t end {0};
        bool enabledSwitch {true};

        while (end != std::string_view::npos)
        {
            if (enabledSwitch)
            {
                end = line.find("don't()", start);
                enabled.push_back(line.substr(start, end - start));
                start = end + 1;
                enabledSwitch = false;
            }

            else
            {
                end = line.find("do()", start);
                start = end + 1;
                enabledSwitch = true;
            }
        }

        return enabled;
    }

    template <typename T>
    int parseAndMultiply(const T& line)
    {
        int result {0};

        std::vector<std::string> enabled {getEnabledInstructions(line)};
        for (const auto& instruction: enabled)
            result += parseLineAndMultiply(instruction);

        return result;
    }
}

#endif // AOC3B_H
