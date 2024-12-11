#ifndef AOC7B_H
#define AOC7B_H

#include "../aoc_7a/permutation.h"
#include "permutation_7b.h"
#include <string_view>
#include <array>
#include <vector>
#include <string>

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

    bool performAddMultCalculation(long long testValue, std::vector<int> numbers, std::vector<std::vector<char>> permutations)
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
    std::vector<size_t> findAddMultTestIndexPasses(std::array<long long, N> testValues, std::array<std::vector<int>, N> numbers)
    {
        std::vector<size_t> successes {};

        for (size_t i{0}; i < testValues.size(); ++i)
        {
            std::vector<std::vector<char>> permutations {permutation::generate(numbers.at(i))};

            if (performAddMultCalculation(testValues.at(i), numbers.at(i), permutations))
                successes.push_back(i);
        }

        return successes;
    }

    template <std::size_t N>
    std::vector<size_t> getOnlyUnsolvedIndices(std::array<long long, N> testValues, std::vector<size_t> addMultIndices)
    {
        std::vector<size_t> unsolved {};

        for (size_t i{0}; i < testValues.size(); ++i)
        {
            bool isSuccess {false};
            for (size_t solvedIndex : addMultIndices)
            {
                if (i == solvedIndex)
                {
                    isSuccess = true;
                    break;
                }
            }
            if (!isSuccess)
                unsolved.push_back(i);
        }

        return unsolved;
    }

    long long concatenateNumbers(long long num1, long long num2)
    {
        std::string concatenatedString;
        concatenatedString.append(std::to_string(num1));
        concatenatedString.append(std::to_string(num2));
        return std::stoll(concatenatedString);
    }

    bool performConcatCalculation(long long testValue, std::vector<int> numbers, std::vector<std::vector<char>> permutations)
    {
        for (std::vector<char> permutation : permutations)
        {
            long long solution {};
            for (size_t i{0}; i < permutation.size(); ++i)
            {
                if (i == 0)
                    solution = numbers.at(i);
                
                if (permutation.at(i) == '(')
                    solution = concatenateNumbers(solution, numbers.at(i + 1));
                else if (permutation.at(i) == '+')
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
    std::vector<size_t> findConcatTestIndexPasses(std::array<long long, N> testValues, std::array<std::vector<int>, N> numbers, std::vector<size_t> addMultIndices)
    {
        std::vector<size_t> successes {};
        std::vector<size_t> unsolvedOnlyIndices {getOnlyUnsolvedIndices(testValues, addMultIndices)};

        for (size_t unsolvedIndex : unsolvedOnlyIndices)
        {
            std::vector<std::vector<char>> permutations {permutation_7b::generateWithConcat(numbers.at(unsolvedIndex))};

            if (performConcatCalculation(testValues.at(unsolvedIndex), numbers.at(unsolvedIndex), permutations))
                successes.push_back(unsolvedIndex);
        }

        return successes;
    }

    template <std::size_t N>
    long long sumSuccesses(std::vector<size_t> addMultSuccessIndices, std::vector<size_t> concatSuccessIndices, std::array<long long, N> testValues)
    {
        long long sum {0};
        for (size_t addMultIndex : addMultSuccessIndices)
        {
            sum += testValues.at(addMultIndex);
        }
        for (size_t concatIndex : concatSuccessIndices)
        {
            sum += testValues.at(concatIndex);
        }
        return sum;
    }

    template <std::size_t N>
    long long parseAndCountTests(std::array<std::string_view, N> lines)
    {
        std::array<long long, N> testValues {getTestValues<N>(lines)};
        std::array<std::vector<int>, N> numbers {getNumbers<N>(lines)};

        std::vector<size_t> addMultSuccessIndices {findAddMultTestIndexPasses<N>(testValues, numbers)};

        std::vector<size_t> concatSuccessIndices {findConcatTestIndexPasses<N>(testValues, numbers, addMultSuccessIndices)};

        return sumSuccesses<N>(addMultSuccessIndices, concatSuccessIndices, testValues);
    }
}

#endif // AOC7B_H
