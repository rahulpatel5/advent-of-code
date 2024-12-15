#ifndef AOC11A_H
#define AOC11A_H

#include <string_view>
#include <vector>
#include <string>
#include <map>
#include <cmath>

/*
parse input and add to a vector
iterate vector 25 times, applying rules to each element for each iteration
count size of vector at the end

CHANGE:
    above approach doesn't work due to vectors getting too long with numbers that are too large (bigger than long long allows)
    size of vector (i.e. number of stones) seems to increase by about 50% for successive iterations. May be able to look for a pattern there?

    there will be patterns in numbers, e.g. 0 (below)
    would it make sense to use dynamic programming & memoisation?
    can memoisation work when there are 3 rules to apply?

    but isn't issue the large numbers? How to handle those
    why do they get arbitrarily large? Because they have an odd number of digits
    is there a way to understand what number of digits result from a product, without calculating it?
    if so, can we just store the number of times we multiply by 2,024 rather than calculating the product?
    scientific notation could work in theory, but we need to know the numbers precisely and floating point precision could cause issues

EXAMPLE PATTERN:
    0 -> 1 -> 2024 -> (20, 24) -> (2, 0, 2, 4) -> (4048, 1, 4048, 8096) ...

DIGIT SIZE:
    1 * 2024 -> [4]
    9 * 2024 -> [5]
    49 * 2024 -> [5]
    50 * 2024 -> [6]
    480 * 2024 -> [6]
    499 * 2024 -> [7]
    
    can we use any significance with 2,024?
    10,000 / 2,024 ~= 4.940711
    if n * 2,024
        where n < 4.94... * 10^a, the number of digits in the product would be (4 + a)
        where n >= 4.94... * 10^a, the number of digits would be (4 + a + 1)
        alternatively, we can say d = number of digits in other number. Then we have (4 + d - 1) for < 4.94, and otherwise (4 + d)

        if we have the digit in scientific notation e.g. 4.99 * 10^2
        then we may be able to look at this portion of the calculation
        i.e. we can do 4.99 * 2024 to get 9.99856 * 10^3
        we then have the exponent as 2 + 3 = 5, i.e. odd number of digits

*/

namespace constants
{
    constexpr int BLINKS {25};
    constexpr int MULTIPLIER {2024};
    constexpr double doubleMULTIPLIER {2024.0};
    constexpr double MANTISSA {10000.0 / MULTIPLIER};
}

namespace aoc11a
{
    std::vector<long long> getInitialStones(std::string_view input)
    {
        std::vector<long long> stones {};
        size_t start {0};
        size_t end {0};
        while (end != std::string_view::npos)
        {
            end = input.find(' ', start);
            long long num {std::stoll(std::string(input.substr(start, end)))};
            stones.push_back(num);
            start = end + 1;
        }
        return stones;
    }

    bool isEvenNumberOfDigits(long long num)
    {
        std::string digit {std::to_string(num)};
        return digit.size() % 2 == 0;
    }

    std::vector<long long> splitAndAddStones(const std::vector<long long>& stones, long long stone)
    {
        std::string originalStone {std::to_string(stone)};
        size_t splitPoint {originalStone.size() / 2};
        std::string leftStone {originalStone.substr(0, splitPoint)};
        std::string rightStone {originalStone.substr(splitPoint)};
        return {std::stoll(leftStone), std::stoll(rightStone)};
    }

    bool willStayOddNumberOfDigits(long long num, int iteration)
    {
        int exponent {(int)std::floor(std::log10(num))};
        double significand {(double)num * std::pow(10, -exponent)};
        for (int i{iteration}; i < constants::BLINKS; ++i)
        {
            significand *= constants::doubleMULTIPLIER;
            int exp {(int)std::floor(std::log10(significand))};
            significand *= std::pow(10, -exp);
            exponent += exp;
            // an even numbered exponent means an odd number of digits
            // account for this when identifying odd number of digits
            if ((exponent + 1) % 2 == 0)
                return false;
        }
        return true;
    }

    std::vector<long long> applyBlinkRules(std::vector<long long>& stones, std::map<long long, std::vector<long long>>& memo, int iteration)
    {
        std::vector<long long> blinkStones {};
        for (size_t i{0}; i < stones.size(); ++i)
        {
            if (memo.count(stones[i]) > 0)
            {
                for (long long value : memo[stones[i]])
                    blinkStones.push_back(value);
            }

            else if (isEvenNumberOfDigits(stones[i]))
            {
                std::vector<long long> split {splitAndAddStones(stones, stones[i])};
                memo[stones[i]] = split;
                for (long long value : split)
                    blinkStones.push_back(value);
            }
            else
            {
                if (willStayOddNumberOfDigits(stones[i], iteration))
                {
                    // this may update the map (memo) value
                    // this is intended, as a number may not reach a point
                    // where splitting is possible after enough iterations
                    memo[stones[i]] = {stones[i]};
                    blinkStones.push_back(stones[i]);
                }
                else
                {
                    memo[stones[i]] = {stones[i] * constants::MULTIPLIER};
                    blinkStones.push_back(stones[i] * constants::MULTIPLIER);
                }
            }
        }
        return blinkStones;
    }

    std::vector<long long> blinkForNewStones(const std::vector<long long>& stones)
    {
        std::vector<long long> blinkStones {stones};
        // using memoisation to store previous values;
        std::map<long long, std::vector<long long>> memo {};
        memo[0] = {1};
        for (int i{0}; i < constants::BLINKS; ++i)
            blinkStones = applyBlinkRules(blinkStones, memo, i);
        return blinkStones;
    }

    int parseAndCountStones(std::string_view input)
    {
        std::vector<long long> startStones {getInitialStones(input)};
        std::vector<long long> finalStones {blinkForNewStones(startStones)};

        return finalStones.size();
    }
}

#endif // AOC11A_H
