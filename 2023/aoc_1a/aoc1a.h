#ifndef AOC1A_H
#define AOC1A_H

#include <array>
#include <string_view>
#include <string>

/*
FOR each line:
    IF first digit:
        SET first character to digit
        SET last character to digit
    IF next digit:
        SET last character to digit

END of line:
    COMBINE digits (concatenate)
    RETURN digit converted to int
*/

namespace aoc1a
{
    using CalInt = long long;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Value = long long;
    using Text = std::string;

    Value getFirstAndLastNumbers(const Text& text)
    {
        char first {};
        char last {};
        bool gotFirstDigit {false};
        for (char c : text)
        {
            if (c >= '0' & c <= '9')
            {
                if (!gotFirstDigit)
                {
                    first = c;
                    last = c;
                    gotFirstDigit = true;
                }
                else
                    last = c;
            }
        }
        
        if (!gotFirstDigit)
            return 0;
        else
        {
            Text digits {first};
            digits += last;
            return std::stoll(digits);
        }
    }

    template <std::size_t L>
    CalInt getCalibrationSum(const Lines<L>& lines)
    {
        CalInt sum {};
        for (const std::string_view line : lines)
        {
            Text text {std::string(line)};
            sum += getFirstAndLastNumbers(text);
        }
        return sum;
    }
}

#endif // AOC1A_H
