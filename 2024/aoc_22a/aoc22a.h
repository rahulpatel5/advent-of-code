#ifndef AOC22A_H
#define AOC22A_H

#include <string_view>
#include <array>
#include <string>

/*
get initial numbers
set up secret rules
apply rules to each number 2,000 times
sum the final numbers for each
*/

namespace Secret
{
    constexpr int MULTIPLIER_FIRST {64};
    constexpr int DIVISOR {32};
    constexpr int MULTIPLIER_LAST {2'048};
    constexpr int PRUNE {16'777'216};
}

namespace aoc22a
{
    using SecretInt = unsigned long long;

    SecretInt mix(SecretInt secret, SecretInt result)
    {
        return secret ^ result;
    }

    SecretInt prune(SecretInt secret)
    {
        return secret % Secret::PRUNE;
    }

    template <std::size_t Titerations>
    SecretInt getLaterSecretNumber(int secret)
    {
        for (size_t i{0}; i < Titerations; ++i)
        {
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
        return secret;
    }

    template <std::size_t N, std::size_t Titerations>
    SecretInt parseAndSumSecretNumbers(const std::array<std::string_view, N>& lines)
    {
        SecretInt sum {0};
        for (std::string_view line : lines)
        {
            int num {std::stoi(std::string(line))};
            sum += getLaterSecretNumber<Titerations>(num);
        }
        return sum;
    }
}

#endif // AOC22A_H
