#ifndef AOC7B_H
#define AOC7B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <algorithm>

#include <iostream>
#include "../../shared/Timer.h"

/*
re-use aoc7a solution to count how many pass using * and +
iterate through remaining unsolved solutions
add permutations including one || operator (note: might be more than one operator?)
use '(' instead, 1) to keep operators as char and 2) as permutations rely on having ASCII codes in ascending order ('(' is before + and *)
iterate through unsolved equations with new permutations
count how many pass, in addition to previous
*/

namespace aoc7b
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

    bool performAddMultCalculation(long long testValue, const std::vector<int>& numbers)
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
                    // there's tricksy cases with + vs * 1
                    if (solution > testValue && std::find(numbers.begin(), numbers.end(), 1) == numbers.end())
                        return false;
                    firstLoop = false;
                }
            } while (std::next_permutation(copyPermutation.begin(), copyPermutation.end()));
        }
        return false;
    }

    long long concatenateNumbers(long long num1, long long num2)
    {
        std::string concatenatedString;
        concatenatedString.append(std::to_string(num1));
        concatenatedString.append(std::to_string(num2));
        return std::stoll(concatenatedString);
    }

    bool performConcatCalculation(long long testValue, const std::vector<int>& numbers)
    {
        std::vector<char> permutation {};
        for (size_t i{1}; i < numbers.size(); ++i)
            permutation.push_back('+');

        for (int k{0}; k < static_cast<int>(permutation.size()); ++k)
        {
            // need to use a character with a lower ASCII value than * or +
            permutation[k] = '(';
            std::vector<char> outerPermutation {permutation};

            for (int j{k}; j < static_cast<int>(permutation.size()); ++j)
            {
                if (j > k)
                    outerPermutation[j] = '*';
                std::vector<char> innerPermutation {outerPermutation};

                do
                {
                    long long solution {};
                    for (size_t i{0}; i < innerPermutation.size(); ++i)
                    {
                        if (i == 0)
                            solution = numbers.at(i);
                        
                        if (innerPermutation.at(i) == '(')
                            solution = concatenateNumbers(solution, numbers.at(i + 1));
                        else if (innerPermutation.at(i) == '+')
                            solution += numbers.at(i + 1);
                        else if (innerPermutation.at(i) == '*')
                            solution *= numbers.at(i + 1);
                    
                        if (solution > testValue)
                            break;
                    }
                    if (solution == testValue)
                        return true;
                } while (std::next_permutation(innerPermutation.begin(), innerPermutation.end()));
            }
        }
        return false;
    }

    template <std::size_t N>
    std::vector<size_t> findTestIndexPasses(const std::array<long long, N>& testValues, const std::array<std::vector<int>, N>& numbers)
    {
        std::vector<size_t> successes {};

        for (size_t i{0}; i < testValues.size(); ++i)
        {
            if (performAddMultCalculation(testValues.at(i), numbers.at(i)))
                successes.push_back(i);
            else if (performConcatCalculation(testValues.at(i), numbers.at(i)))
                successes.push_back(i);
        }

        return successes;
    }

    template <std::size_t N>
    long long sumSuccesses(const std::vector<size_t>& addSuccessIndices, const std::array<long long, N>& testValues)
    {
        long long sum {0};
        for (size_t successIndex : addSuccessIndices)
            sum += testValues.at(successIndex);
        return sum;
    }

    template <std::size_t N>
    long long parseAndCountTests(const std::array<std::string_view, N>& lines)
    {
        std::array<long long, N> testValues {getTestValues<N>(lines)};
        std::array<std::vector<int>, N> numbers {getNumbers<N>(lines)};

        std::vector<size_t> addSuccessIndices {findTestIndexPasses<N>(testValues, numbers)};

        return sumSuccesses<N>(addSuccessIndices, testValues);
    }
}

#endif // AOC7B_H
