#ifndef AOC11A_H
#define AOC11A_H

#include <string_view>
#include <vector>
#include <string>

#include <iostream>

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
*/

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

    bool isEvenNumberOfDigits(int num)
    {
        std::string digit {std::to_string(num)};
        return digit.size() % 2 == 0;
    }

    void splitAndAddStones(std::vector<long long>& stones, long long stone)
    {
        std::string originalStone {std::to_string(stone)};
        size_t splitPoint {originalStone.size() / 2};
        std::string leftStone {originalStone.substr(0, splitPoint)};
        stones.push_back(std::stoll(leftStone));
        std::string rightStone {originalStone.substr(splitPoint)};
        stones.push_back(std::stoll(rightStone));
    }

    std::vector<long long> applyBlinkRules(const std::vector<long long>& stones)
    {
        std::vector<long long> blinkStones {};
        constexpr int multiplier {2024};
        for (size_t i{0}; i < stones.size(); ++i)
        {
            if (stones[i] == 0)
                blinkStones.push_back(1);
            else if (isEvenNumberOfDigits(stones[i]))
                splitAndAddStones(blinkStones, stones[i]);
            else
                blinkStones.push_back(stones[i] * multiplier);
        }
        return blinkStones;
    }

    std::vector<long long> blinkForNewStones(const std::vector<long long>& stones, int blinks)
    {
        std::vector<long long> blinkStones {stones};
        for (int i{0}; i < blinks; ++i)
        {
            std::cout << "iteration: " << i << '\n';
            std::cout << blinkStones.size() << '\n';
            blinkStones = applyBlinkRules(blinkStones);
        }
        return blinkStones;
    }

    int parseAndCountStones(std::string_view input, int blinks)
    {
        std::vector<long long> startStones {getInitialStones(input)};
        std::vector<long long> finalStones {blinkForNewStones(startStones, blinks)};

        return finalStones.size();
    }
}

#endif // AOC11A_H
