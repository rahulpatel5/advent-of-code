#ifndef AOC7A_H
#define AOC7A_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <algorithm>

/*
extract test values and (input) numbers

create permutations of * and +, but ad hoc
means less memory use if stopping before using all permutations
*/

namespace aoc7a
{
    template <std::size_t N>
    std::array<long long, N> getTestValues(const std::array<std::string_view, N>& lines)
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
    std::array<std::vector<int>, N> getNumbers(const std::array<std::string_view, N>& lines)
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

    bool performCalculation(long long testValue, const std::vector<int>& numbers)
    {
        // initial check that all multiplication is enough
        long long maxValue {numbers.at(0)};
        for (size_t fin{1}; fin < numbers.size(); ++fin)
        {
            // handle unique situation where adding 1 does more than mult
            if (numbers.at(fin) == 1)
                maxValue += 1;
            else
                maxValue *= numbers.at(fin);
        }
        if (maxValue < testValue)
            return false;
        else if (maxValue == testValue)
            return true;

        // we need operators to apply
        // we'll use all + as default, as * has a lower ASCII value
        std::vector<char> permutation {};
        for (size_t i{1}; i < numbers.size(); ++i)
            permutation.push_back('+');

        bool firstLoop {true};
        for (int j{-1}; j < static_cast<int>(permutation.size()); ++j)
        {
            if (j > -1)
                permutation[j] = '*';
            std::vector<char> copyPermutation {permutation};

            do
            {
                long long solution {};
                for (size_t i{0}; i < copyPermutation.size(); ++i)
                {
                    if (i == 0)
                        solution = numbers.at(i);
                    if (copyPermutation.at(i) == '+')
                        solution += numbers.at(i + 1);
                    else if (copyPermutation.at(i) == '*')
                        solution *= numbers.at(i + 1);
                    
                    if (solution > testValue)
                        break;
                    }
                if (solution == testValue)
                    return true;
                
                // first loop is all adding. If that's too large, then false
                if (firstLoop)
                {
                    if (solution > testValue)
                        return false;
                    firstLoop = false;
                }
            } while (std::next_permutation(copyPermutation.begin(), copyPermutation.end()));
        }

        return false;
    }

    template <std::size_t N>
    std::vector<long long> findTestPasses(const std::array<long long, N>& testValues, const std::array<std::vector<int>, N>& numbers)
    {
        std::vector<long long> successes {};

        for (size_t i{0}; i < testValues.size(); ++i)
        {
            if (performCalculation(testValues.at(i), numbers.at(i)))
                successes.push_back(testValues.at(i));
        }

        return successes;
    }

    long long sumSuccesses(const std::vector<long long>& successes)
    {
        long long sum {0};
        for (long long success : successes)
        {
            sum += success;
        }
        return sum;
    }

    template <std::size_t N>
    long long parseAndCountTests(const std::array<std::string_view, N>& lines)
    {
        std::array<long long, N> testValues {getTestValues<N>(lines)};
        std::array<std::vector<int>, N> numbers {getNumbers<N>(lines)};

        std::vector<long long> successTests {findTestPasses<N>(testValues, numbers)};

        return sumSuccesses(successTests);
    }
}

#endif // AOC7A_H
