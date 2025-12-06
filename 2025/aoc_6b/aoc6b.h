#ifndef AOC6B_H
#define AOC6B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <ranges>
#include <cassert>

/*
READ values for each column
CONVERT each column into a number

FOR each set of numbers:
    APPLY the maths operation (bottom of left-most column)
    ADD the solution to the final sum

RETURN the final sum
*/

namespace aoc6b
{
    using MathsInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using NumberString = std::string;
    using Problems = std::vector<NumberString>;

    template <std::size_t L>
    Problems getValuesFromInput(const Lines<L>& lines)
    {
        // set up the object
        Problems problems (lines.at(0).size());

        for (std::string_view line : lines)
        {
            std::string row { line };
            assert(row.size() == lines.at(0).size() && "Have a row that has a different number of characters compared to the first row.\n");
            for (size_t j{0}; j < row.size(); ++j)
            {
                problems[j] += row.at(j);
            }
        }
        return problems;
    }

    MathsInt getGrandTotal(const Problems& problems)
    {
        MathsInt total {0};
        MathsInt singleSolution {0};

        char operation {};
        for (const NumberString& numStr : problems)
        {
            if (numStr.empty() || numStr.find_first_of("0123456789*+") == std::string::npos)
                continue;
            
            // if reach a maths operator, start new operation
            // and end old one, if there was one
            if (numStr.find('+') != std::string_view::npos || numStr.find('*') != std::string_view::npos)
            {
                if (total > 0 || singleSolution > 0)
                {
                    total += singleSolution;
                    singleSolution = 0;
                }

                operation = numStr.back();
                if (operation == '+')
                    singleSolution = 0;
                else if (operation == '*')
                    singleSolution = 1;
                else
                    throw std::invalid_argument("Got an unexpected operation (not '+' or '*')");
                
                NumberString lineNum { numStr.substr(0, numStr.size() - 1) };
                if (operation == '+')
                    singleSolution += std::stoll(lineNum);
                else if (operation == '*')
                    singleSolution *= std::stoll(lineNum);
            }
            else
            {
                NumberString lineNum { numStr.substr(0, numStr.size() - 1) };
                if (operation == '+')
                    singleSolution += std::stoll(lineNum);
                else if (operation == '*')
                    singleSolution *= std::stoll(lineNum);
            }
        }
        // need to add the final solution
        return total + singleSolution;
    }

    template <std::size_t L>
    MathsInt doCephalopodMaths(const Lines<L>& lines)
    {
        Problems problems { getValuesFromInput<L>(lines) };
        
        return getGrandTotal(problems);
    }
}

#endif // AOC6B_H
