#ifndef AOC3A_H
#define AOC3A_H

#include <string>
#include <sstream>

namespace aoc3a
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
        std::string input {line};
        size_t start {0};
        size_t end {0};
        int product {0};

        while ((end = input.find("mul(", start)) != std::string_view::npos)
        {
            size_t nextBracket {input.find_first_of(')', end)};
            if (nextBracket - end <= 11)
                product += multiply(input.substr(end, nextBracket));
            start = end + 1;
        }

        return product;
    }

    template <typename T>
    int parseAndMultiply(const T& lines)
    {
        int result {0};

        // use this line with a one line input (e.g. the test input)
        // result += parseLineAndMultiply(lines);
        
        for (const auto& line : lines)
        {
            result += parseLineAndMultiply(line);
        }

        return result;
    }
}

#endif // AOC3A_H
