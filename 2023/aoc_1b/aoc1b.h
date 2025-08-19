#ifndef AOC1B_H
#define AOC1B_H

#include <array>
#include <string_view>
#include <string>

/*
FOR each character in the line:
    IF first digit OR word spelling number:
        SET first value to corresponding number
        SET last value to corresponding number
    IF next digit OR word spelling number:
        SET last value to corresponding number

END of line:
    COMBINE digits (concatenate)
    RETURN digit converted to int
*/

namespace aoc1b
{
    using CalInt = long long;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Value = long long;
    using Text = std::string;

    constexpr std::array<Text, 9> numberWords {
        "one",
        "two",
        "three",
        "four",
        "five",
        "six",
        "seven",
        "eight",
        "nine",
    };
    constexpr std::array<char, 9> wordChars {
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
    };

    char convertWordToDigit(const Text& text)
    {
        for (size_t i{0}; i < numberWords.size(); ++i)
        {
            if (text.size() >= numberWords.at(i).size() && text.substr(0, numberWords.at(i).size()) == numberWords.at(i))
                return wordChars.at(i);
        }
        
        // sentinel value
        return ' ';
    }

    Value getFirstAndLastNumbers(const Text& text)
    {
        char first {};
        char last {};
        bool gotFirstDigit {false};
        for (size_t i{0}; i < text.size(); ++i)
        {
            if (text.at(i) >= '0' & text.at(i) <= '9')
            {
                if (!gotFirstDigit)
                {
                    first = text.at(i);
                    last = text.at(i);
                    gotFirstDigit = true;
                }
                else
                    last = text.at(i);
            }
            else
            {
                char wordNumber {convertWordToDigit(text.substr(i))};
                if (wordNumber != ' ')
                {
                    if (!gotFirstDigit)
                    {
                        first = wordNumber;
                        last = wordNumber;
                        gotFirstDigit = true;
                    }
                    else
                        last = wordNumber;
                }
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

#endif // AOC1B_H
