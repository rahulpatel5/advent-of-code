#ifndef AOC22B_H
#define AOC22B_H

#include <string_view>
#include <array>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

/*
re-use solution to part 1
this time collect the sequence of numbers
or, specifically, get the final digit (% 10?)
get the difference with the final digit of the previous number
figure out a way to test different iterations of numbers
since we can only have single digit numbers, the difference is [-9, 9]
iterate all sequences of 4 and return largest haul

getting an answer that is too low
spotted I set up combinations wrong - missed some (where numbers the same)
*/

namespace Secret
{
    constexpr int MULTIPLIER_FIRST {64};
    constexpr int DIVISOR {32};
    constexpr int MULTIPLIER_LAST {2'048};
    constexpr int PRUNE {16'777'216};
}

namespace aoc22b
{
    using SecretInt = unsigned long long;
    using BananaInt = unsigned long long;
    template <std::size_t N>
    using Prices = std::array<int, N>;
    template <std::size_t N>
    using AllPrices = std::vector<Prices<N>>;
    template <std::size_t N>
    using Patterns = std::array<int, N>;
    template <std::size_t N>
    using AllPatterns = std::vector<Patterns<N>>;
    using Iteration = std::vector<int>;
    using Iterations = std::vector<Iteration>;

    SecretInt mix(SecretInt secret, SecretInt result)
    {
        return secret ^ result;
    }

    SecretInt prune(SecretInt secret)
    {
        return secret % Secret::PRUNE;
    }

    template <std::size_t Titerations>
    Prices<Titerations> getPrices(int secret)
    {
        Prices<Titerations> prices {};
        for (size_t i{0}; i < Titerations; ++i)
        {
            // collect price first
            prices[i] = secret % 10;
            // first stage of secret process
            secret = mix(secret, secret * Secret::MULTIPLIER_FIRST);
            secret = prune(secret);
            // second stage of secret process
            secret = mix(secret, secret / Secret::DIVISOR);
            secret = prune(secret);
            // third stage of secret process
            secret = mix(secret, secret * Secret::MULTIPLIER_LAST);
            secret = prune(secret);
        }
        return prices;
    }

    template <std::size_t Titerations>
    Patterns<Titerations - 1> getPatterns(const Prices<Titerations>& prices)
    {
        Patterns<Titerations - 1> patterns {};
        for (size_t i{1}; i < prices.size(); ++i)
        {
            patterns[i - 1] = prices[i] - prices[i - 1];
        }
        return patterns;
    }

    Iterations getCombinations(int min, int max, int comboSize)
    {
        Iterations combinations {};
        // this is a garbage way to get combinations
        // but I'll leave setting up a better way to another time
        for (int a{min}; a <= max; ++a)
        {
            for (int b{a}; b <= max; ++b)
            {
                for (int c{b}; c <= max; ++c)
                {
                    for (int d{c}; d <= max; ++d)
                    {
                        combinations.push_back({a, b, c, d});
                    }
                }
            }
        }
        return combinations;
    }

    template <std::size_t Titerations, int sequences>
    BananaInt soldBananas(const Iteration& combination, const Patterns<Titerations - 1>& patterns, const Prices<Titerations>& prices)
    {
        BananaInt bananas {0};
        auto it {std::search(patterns.begin(), patterns.end(), combination.begin(), combination.end())};
        if (it != patterns.end())
        {
            size_t match {static_cast<size_t>(std::distance(patterns.begin(), it))};
            bananas = prices[match + 4];
        }
        return bananas;
    }

    // sequences is (mostly) not used here
    // but ideally would be where generating combinations generically
    template <std::size_t Titerations, int sequences>
    BananaInt iterateSequencesAndGetMostBananas(const AllPrices<Titerations>& allPrices, const AllPatterns<Titerations - 1>& allPatterns)
    {
        // price changes can be between -9 and 9 (inclusive)
        Iterations combinations {getCombinations(-9, 9, 4)};
        BananaInt mostBananas {0};
        for (Iteration& combination : combinations)
        {
            while (std::next_permutation(combination.begin(), combination.end()))
            {
                BananaInt total {0};
                for (size_t i{0}; i < allPatterns.size(); ++i)
                {
                    total += soldBananas<Titerations, sequences>(combination, allPatterns[i], allPrices[i]);
                    // if can't match mostBananas, skip to save time
                    if (mostBananas >= total + (allPatterns.size() - (i + 1)) * 9)
                        break;
                }
                if (total > mostBananas)
                    mostBananas = total;
            }
        }
        return mostBananas;
    }

    template <std::size_t N, std::size_t Titerations, int sequences>
    BananaInt parseAndGetBananas(const std::array<std::string_view, N>& lines)
    {
        AllPrices<Titerations> allPrices {};
        AllPatterns<Titerations - 1> allPatterns {};
        for (size_t i{0}; i < lines.size(); ++i)
        {
            int num {std::stoi(std::string(lines[i]))};
            Prices<Titerations> priceChanges {getPrices<Titerations>(num)};
            allPrices.push_back(priceChanges);
            Patterns<Titerations - 1> pricePatterns {getPatterns<Titerations>(priceChanges)};
            allPatterns.push_back(pricePatterns);
        }
        return iterateSequencesAndGetMostBananas<Titerations, sequences>(allPrices, allPatterns);
    }
}

#endif // AOC22B_H
