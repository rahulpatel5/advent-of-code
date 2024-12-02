#ifndef AOC10A_H
#define AOC10A_H

#include <vector>

namespace aoc10a
{
    template <typename T>
    std::vector<int> readInput(const T& input)
    {
        std::vector<int> output {};
        for (const auto& digit : input)
        {
            int number {digit - '0'};
            output.push_back(number);
        }
        return output;
    }

    std::vector<int> lookAndSee(std::vector<int> digits)
    {
        std::vector<int> newDigits {};
        int value {digits[0]};
        int repeats {1};
        for (auto i{1}; i < digits.size(); ++i)
        {
            if (digits[i] == value)
                ++repeats;
            else
            {
                newDigits.push_back(repeats);
                repeats = 1;
                newDigits.push_back(value);
                value = digits[i];
            }
        }
        // need to handle the last set of values separately
        newDigits.push_back(repeats);
        newDigits.push_back(value);

        return newDigits;
    }

    std::vector<int> iterateLookAndSay(const std::vector<int>& digits, int iterations)
    {
        std::vector<int> newDigits {digits};
        for (auto i{0}; i < iterations; ++i)
        {
            newDigits = lookAndSee(newDigits);
        }
        return newDigits;
    }

    template <typename T>
    std::vector<int> playLookAndSay(const T& input, int iterations)
    {
        std::vector<int> initialDigits {readInput(input)};
        std::vector<int> finalDigits {iterateLookAndSay(initialDigits, iterations)};
        return {finalDigits};
    }
}

#endif // AOC10A_H
