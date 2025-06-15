#ifndef AOC7B_H
#define AOC7B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <future>
#include <stdexcept>

/*
FOR each test value
    TEST each permutation of + and * and || for those numbers
        IF the calculation equals the test value
            THEN mark this test value as a success
            ADD the test value to the sum of successes
            MOVE to the next test value
        ELSE
            TRY the next permutation

RETURN sum of successes

use parallelisation to speed up code
*/

namespace aoc7b
{
    static double tim {0};
    using Result = long long;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using TestValue = long long;
    template <std::size_t N>
    using TestValues = std::array<TestValue, N>;

    using Number = int;
    using Numbers = std::vector<Number>;
    template <std::size_t N>
    using AllNumbers = std::array<Numbers, N>;

    using Permutation = std::vector<char>;

    template <std::size_t N>
    TestValues<N> getTestValues(const Lines<N>& lines)
    {
        TestValues<N> testVals {};

        for (size_t i{0}; i < lines.size(); ++i)
        {
            size_t colon {lines.at(i).find(':')};
            TestValue test {std::stoll(std::string(lines.at(i).substr(0, colon)))};
            testVals[i] = test;
        }

        return testVals;
    }

    template <std::size_t N>
    AllNumbers<N> getNumbers(const Lines<N>& lines)
    {
        AllNumbers<N> outerNumbers {};

        for (size_t i{0}; i < lines.size(); ++i)
        {
            Numbers innerNumbers {};
            
            size_t start {lines.at(i).find(':') + 2};
            size_t end {0};
            
            while (end != std::string_view::npos)
            {
                end = lines.at(i).find(' ', start);
                Number num {std::stoi(std::string(lines.at(i).substr(start, end - start)))};
                innerNumbers.push_back(num);
                start = end + 1;
            }
            outerNumbers[i] = innerNumbers;
        }

        return outerNumbers;
    }

    long long findDigits(long long num)
    {
        if (num < 10)
            return 10;
        else if (num < 100)
            return 100;
        else if (num < 1'000)
            return 1'000;
        else
            throw std::out_of_range("Number larger than expected.\n");
    }

    long long concatenateNumbers(long long num1, long long num2)
    {
        long long tensMultiplier {findDigits(num2)};
        return (tensMultiplier * num1 + num2);
    }

    bool performConcatCalculation(TestValue testValue, const Numbers& numbers, const Permutation& permutation)
    {
        long long solution {};
        for (size_t i{0}; i < permutation.size(); ++i)
        {
            if (i == 0)
                solution = numbers.at(i);
            
            if (permutation.at(i) == '(')
            {
                solution = concatenateNumbers(solution, numbers.at(i + 1));
            }
            else if (permutation.at(i) == '+')
                solution += numbers.at(i + 1);
            else if (permutation.at(i) == '*')
                solution *= numbers.at(i + 1);
            
            if (solution > testValue)
                return false;
        }

        return (solution == testValue);
    }

    bool recursivePermutation(TestValue testValue, const Numbers& numbers, Permutation& perm, size_t index)
    {
        if (index == perm.size())
            return performConcatCalculation(testValue, numbers, perm);
        
        // this order of operations is the fastest, from testing
        for (char oper : {'*', '+', '('})
        {
            perm[index] = oper;

            if(recursivePermutation(testValue, numbers, perm, index + 1))
                return true;
        }

        return false;
    }

    template <std::size_t N>
    Result totalSuccesses(const TestValues<N>& testValues, const AllNumbers<N>& numbers)
    {
        std::vector<std::future<Result>> futures {};
        Result result {0};

        for (size_t i{0}; i < testValues.size(); ++i)
        {
            futures.push_back(std::async(std::launch::async, [=]() -> Result {
                Permutation perm (numbers.at(i).size() - 1, '+');
                if (recursivePermutation(testValues.at(i), numbers.at(i), perm, 0)) {
                    return testValues.at(i);
                }
                return 0ll;
            }));
        }

        for (auto& f : futures)
            result += f.get();
        
        return result;
    }

    template <std::size_t N>
    Result parseAndCountTests(const Lines<N>& lines)
    {
        TestValues testValues {getTestValues<N>(lines)};
        AllNumbers numbers {getNumbers<N>(lines)};

        return totalSuccesses<N>(testValues, numbers);
    }
}

#endif // AOC7B_H
