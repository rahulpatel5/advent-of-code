#ifndef AOC20A_H
#define AOC20A_H

#include <string_view>
#include <string>
#include <cmath>
#include <stdexcept>
#include <limits>

/*
could this be a problem about factors?
factors are there, but not sure they easily help with the solution

it's simple enough to solve this problem
but the challenge is how to do so quickly/efficiently
think the most justifiable way is to use 12 having lots of factors
by extension, it feels reasonable to assume the solution % 12 == 0
this cuts down how many numbers we check by a lot
(it's guaranteed that the solution is even, if nothing else)

FIND consecutive multiples of 12 that solution sits between
ITERATE through numbers divisible by 12 in that range
IF the number has a sum of factors reaching the target
    THEN return the number as the solution
ELSE move to the next number divisible by 12

failed ideas:
    find a range in which the solution may lie, using powers of 10. This didn't limit the range (even after refining the range) to one that could be traversed in a reasonable time. Note: the issue may instead have been that 10 has fewer factors than 12
*/

namespace aoc20a
{
    using HouseInt = long long;
    // each elf gives 10x their number
    constexpr int GIFT_MULTIPLIER {10};

    using TargetStr = std::string_view;
    using Target = long long;
    using TargetRange = std::pair<Target, Target>;

    using SquareRoot = double;
    using PowersOfTwelve = long long;

    Target sumOfFactors(Target n)
    {
        Target sum {};
        // save time by collecting factors up to the square root
        SquareRoot root {std::sqrt(n)};
        bool isOdd {n % 2 != 0};
        for (Target x{1}; x <= root; ++x)
        {
            // factors for odd numbers must be odd themselves
            if (isOdd && x % 2 == 0)
                continue;
            
            if (n % x == 0)
            {
                sum += x + n / x;
            }
        }
        return sum * GIFT_MULTIPLIER;
    }

    TargetRange getRange(Target target)
    {
        PowersOfTwelve powers {12};
        Target lower {};
        Target upper {};
        while (powers < std::numeric_limits<long long>::max() / 12)
        {
            if (upper == 0)
            {
                upper = sumOfFactors(powers);
                continue;
            }
            lower = upper;
            powers *= 12;
            upper = sumOfFactors(powers);

            if (upper >= target)
            {
                return {powers / 12, powers};
            }
        }
        throw std::out_of_range("Could not find a valid range.\n");
    }

    HouseInt findHouseNumber(Target target, const TargetRange& range)
    {
        for (Target n{range.first}; n <= range.second; n += 12)
        {
            if (sumOfFactors(n) >= target)
                return n;
        }
        throw std::out_of_range("Did not find a solution.\n");
    }

    HouseInt getLowestHouseNumber(const TargetStr& targetStr)
    {
        Target target {std::stoll(std::string(targetStr))};

        TargetRange initialRange {getRange(target)};

        return findHouseNumber(target, initialRange);
    }
}

#endif // AOC20A_H
