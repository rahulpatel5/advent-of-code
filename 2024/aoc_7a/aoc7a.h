#ifndef AOC7A_H
#define AOC7A_H

#include "permutation.h"
#include <string_view>
#include <array>
#include <vector>
#include <string>

/*
extract test values and (input) numbers
create permutations of * and +
iterate permutations and count how many pass (at least once)
*/

namespace aoc7a
{
    template <std::size_t N>
    std::array<long long, N> getTestValues(std::array<std::string_view, N> lines)
    {
        std::array<long long, N> testVals {};

        for (size_t i{0}; i < lines.size(); ++i)
        {
            size_t colon {lines.at(i).find(':')};
            long long test {std::stoll(std::string(lines.at(i).substr(0, colon)))};
            testVals[i] = test;
        }

        return testVals;
    }

    template <std::size_t N>
    std::array<std::vector<int>, N> getNumbers(std::array<std::string_view, N> lines)
    {
        std::array<std::vector<int>, N> outerNumbers {};

        for (size_t i{0}; i < lines.size(); ++i)
        {
            std::vector<int> innerNumbers {};
            
            size_t start {lines.at(i).find(':') + 2};
            size_t end {0};
            
            while (end != std::string_view::npos)
            {
                end = lines.at(i).find(' ', start);
                int num {std::stoi(std::string(lines.at(i).substr(start, end - start)))};
                innerNumbers.push_back(num);
                start = end + 1;
            }
            outerNumbers[i] = innerNumbers;
        }

        return outerNumbers;
    }

    bool performCalculation(long long testValue, std::vector<int> numbers, std::vector<std::vector<char>> permutations)
    {
        for (std::vector<char> permutation : permutations)
        {
            long long solution {};
            for (size_t i{0}; i < permutation.size(); ++i)
            {
                if (i == 0)
                    solution = numbers.at(i);
                if (permutation.at(i) == '+')
                    solution += numbers.at(i + 1);
                else if (permutation.at(i) == '*')
                    solution *= numbers.at(i + 1);
            }
            if (solution == testValue)
                return true;
        }

        return false;
    }

    template <std::size_t N>
    std::vector<long long> findTestPasses(std::array<long long, N> testValues, std::array<std::vector<int>, N> numbers)
    {
        std::vector<long long> successes {};

        for (size_t i{0}; i < testValues.size(); ++i)
        {
            std::vector<std::vector<char>> permutations {permutation::generate(numbers.at(i))};

            if (performCalculation(testValues.at(i), numbers.at(i), permutations))
                successes.push_back(testValues.at(i));
        }

        return successes;
    }

    long long sumSuccesses(std::vector<long long> successes)
    {
        long long sum {0};
        for (long long success : successes)
        {
            sum += success;
        }
        return sum;
    }

    template <std::size_t N>
    long long parseAndCountTests(std::array<std::string_view, N> lines)
    {
        std::array<long long, N> testValues {getTestValues<N>(lines)};
        std::array<std::vector<int>, N> numbers {getNumbers<N>(lines)};

        std::vector<long long> successTests {findTestPasses<N>(testValues, numbers)};

        return sumSuccesses(successTests);
    }
}

#endif // AOC7A_H
