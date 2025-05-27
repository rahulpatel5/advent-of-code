#ifndef AOC20B_H
#define AOC20B_H

#include <string_view>
#include <string>
#include <stdexcept>
#include <cmath>
#include <limits>

/*
the differences with the part 1 problem
    different gift multiplier
    limit to number of houses visited

this seems to mean factors are no longer relevant at a point
this means the solution will be higher
it might not be possible to use the same approaches to save time

except most of them can be
still look up to root n, as faster to find lower factors
check that lower or complimentary factors are >50x n

what can we assume about the solution to save time?
feels safe to assume it must be an even number
can we still apply the same logic with multiple of 12?
I think it still makes sense
    we need lots of high factors to reach the target sooner
    having a factor of 12 means 2, 3, 4, 6 too
    the complimentary factors of those will be large

FIND consecutive multiples of 12 that solution sits between
ITERATE through numbers divisible by 12 in that range
IF the number has a sum of factors within n reaching the target
    THEN return the number as the solution
ELSE move to the next number divisible by 12
*/

namespace aoc20b
{
    using HouseInt = long long;
    // each elf gives 11x their number
    constexpr int GIFT_MULTIPLIER {11};
    // gifts stop after 50
    constexpr int GIFT_END {50};
    // multiplier we're using to find a suitable range
    constexpr int RANGE_MULTIPLIER {12};

    using TargetStr = std::string_view;
    using Target = long long;
    using TargetRange = std::pair<Target, Target>;

    using SquareRoot = double;
    using SquareRoot = double;
    using PowersOf = long long;

    Target sumOfLargeFactors(Target n)
    {
        Target sum {};
        // save time by collecting factors up to the square root
        SquareRoot root {std::sqrt(n)};
        bool isOdd {n % 2 != 0};
        for (Target x{1}; x <= root; ++x)
        {
            // factors for odd numbers must be odd themselves
            // bit pointless as we're not searching odd numbers
            if (isOdd && x % 2 == 0)
                continue;
            
            if (n % x == 0)
            {
                if (GIFT_END * x >= n)
                    sum += x;
                if (GIFT_END * (n / x) >= n)
                    sum += n / x;
            }
        }
        return sum * GIFT_MULTIPLIER;
    }

    TargetRange getRange(Target target)
    {
        PowersOf powers {RANGE_MULTIPLIER};
        Target lower {};
        Target upper {};
        while (powers < std::numeric_limits<long long>::max() / RANGE_MULTIPLIER)
        {
            if (upper == 0)
            {
                upper = sumOfLargeFactors(powers);
                continue;
            }
            lower = upper;
            powers *= RANGE_MULTIPLIER;
            upper = sumOfLargeFactors(powers);

            if (upper >= target)
            {
                return {powers / RANGE_MULTIPLIER, powers};
            }
        }
        throw std::out_of_range("Could not find a valid range.\n");
    }

    HouseInt findHouseNumber(Target target, const TargetRange& range)
    {
        for (Target n{range.first}; n <= range.second; n += RANGE_MULTIPLIER)
        {
            if (sumOfLargeFactors(n) >= target)
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

#endif // AOC20B_H
