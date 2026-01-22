#ifndef AOC10B_H
#define AOC10B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
#include <ranges>

#include <iostream>

/*
FOR each machine:
    FOR each button combination:
        IF combo meets joltage requirement AND combo <fewest presses:
            STORE as fewest presses
    ADD fewest presses to total

RETURN total

use permutations to try different sequences of button combinations
for now, assume that if any longer button sequence can be pressed
that this will lead to the fewest button presses
(there could be situations where skipping longer ones is better,
but don't want to get into that headache unless I have to)

there may be a smarter way to do this
have information about which combinations contribute for each button
and we have the number of presses we need for each joltage, e.g.
{5, 5, 5, 5}
(0, 1), (1, 2, 3), (1, 3), (2, 3)
since there's only one button affecting 0, we know that has to be pressed
if it's split between two buttons, some combo of the two must add up to 5
i.e. can form and solve algebraic equations to get solutions
assume equations will be linear, so only have one solution

there are sometimes more variables than equations
so Gaussian elimination or Cramer's rule won't work for these equations
note: some variables (button presses) may be 0

I'll start with using an iterative approach to try to find solutions
doing this checking each iteration doesn't work - it takes too long
try an approach that jumps around to find a solution, e.g. using gradients

use max amount for each set of buttons AND
that sum of sets of buttons must add up to certain amounts (using joltages)
find one solution that fits the sum criteria THEN
look at difference with puzzle and how sets of buttons affect output
to figure out what to change to fully solve a sub-problem
the main issue left will be getting the fewest number of presses BUT
should be ok since have ordered sets by number of buttons
so can prioritise the earlier sets
*/

namespace aoc10b
{
    using PressInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Button = long long;
    using Buttons = std::vector<Button>;
    using Schematic = std::vector<Buttons>;
    using Joltage = long long;
    using Joltages = std::vector<Joltage>;

    using SchematicSizes = std::map<size_t, Schematic>;
    using SchematicMap = std::map<size_t, Buttons>;
    using Vector = std::vector<long long>;

    void getMachineInfo(const std::string& line, Schematic& schematic, Joltages& joltages)
    {
        size_t index {0};
        size_t firstBracket { line.find('(') };
        size_t lastBracket { line.find_last_of(')') };
        std::string schematicRange { line.substr(firstBracket, lastBracket - firstBracket) };
        while (index != std::string::npos)
        {
            size_t open { schematicRange.find('(', index) };
            size_t close { schematicRange.find(')', index) };
            if (close != -1)
                index = close + 1;
            else
                index = close;

            std::string buttons { schematicRange.substr(open, close - open) };
            Buttons innerButtons {};
            size_t current {1};
            while (current != std::string::npos)
            {
                size_t comma { buttons.find(',', current) };
                std::string numStr {};
                if (comma != -1)
                    numStr = buttons.substr(current, comma - current);
                else
                    numStr = buttons.substr(current);
                innerButtons.push_back(std::stoll(numStr));
                current = comma + 1;
                if (current == std::string::npos || current == 0)
                    break;
            }
            schematic.push_back(innerButtons);
        }

        size_t openBracket { line.find('{') };
        size_t closeBracket { line.find('}') };
        std::string joltageStr { line.substr(openBracket, closeBracket - openBracket) };
        size_t start {0};
        while (start != std::string::npos)
        {
            size_t joltComma { joltageStr.find(',', start + 1) };
            std::string num { joltageStr.substr(start + 1, joltComma - (start + 1)) };
            joltages.push_back(std::stoll(num));
            start = joltComma;
        }
    }

    bool matchesSolution(const Vector& testSolution, const SchematicMap& schematicMap, const Joltages& solution)
    {
        Joltages testJoltages(solution.size(), 0);
        for (const auto& [key, values] : schematicMap)
        {
            for (Button b : values)
            {
                testJoltages[b] += testSolution[key];
                if (testJoltages.at(b) > solution.at(b))
                {
                    return false;
                }
            }
        }
        return testJoltages == solution;
    }

    Joltages getTestJoltages(const Vector& testSolution, const Joltages& solution, const SchematicMap& schematicMap)
    {
        Joltages test(solution.size(), 0);
        for (size_t i{0}; i < testSolution.size(); ++i)
        {
            for (Button b : schematicMap.at(i))
            {
                test[b] += testSolution.at(i);
            }
        }
        return test;
    }

    long long getDifference(const Vector& testSolution, const Joltages& solution, const SchematicMap& schematicMap)
    {
        Joltages current { getTestJoltages(testSolution, solution, schematicMap) };

        long long difference {0};
        for (size_t j{0}; j < solution.size(); ++j)
        {
            // take the square to account for negative differences
            difference += (current.at(j) - solution.at(j)) * (current.at(j) - solution.at(j));
        }
        return difference;
    }

    PressInt getNumberOfPresses(const Vector& testSolution)
    {
        return std::accumulate(testSolution.begin(), testSolution.end(), 0);
    }

    PressInt solveIteratively(const Joltages& solution, const Schematic& schematic, PressInt theoreticalMin)
    {
        // put button combinations into size order
        SchematicSizes sizes {};
        for (const Buttons& b : schematic)
        {
            sizes[b.size()].push_back(b);
        }
        SchematicMap schematicMap {};
        size_t index {0};
        for (const auto& [key, values] : sizes | std::ranges::views::reverse)
        {
            for (const Buttons& buttons : values)
            {
                schematicMap[index] = buttons;
                ++index;
            }
        }

        Vector maxMultipliers {};
        for (const auto& [k, but] : schematicMap)
        {
            int lowestMax {-1};
            for (Button b : but)
            {
                if (lowestMax == -1 || solution.at(b) < lowestMax)
                    lowestMax = solution.at(b);
            }
            maxMultipliers.push_back(lowestMax);
        }

        Vector testSolution(schematic.size(), 0);

        // ADD HERE

        return 0;
    }

    template <std::size_t L>
    PressInt getFewestPresses(const Lines<L>& lines)
    {
        PressInt total {0};
        for (std::string_view line : lines)
        {
            if (line.find('(') == std::string_view::npos)
                continue;
            Schematic schematic {};
            Joltages joltages {};
            getMachineInfo(std::string(line), schematic, joltages);

            // estimate minimum button presses
            PressInt maxJoltage { *std::max_element(joltages.begin(), joltages.end()) };
            PressInt joltageSum { std::accumulate(joltages.begin(), joltages.end(), 0) };
            PressInt mostButtons {};
            for (const Buttons& b : schematic)
            {
                if (b.size() > mostButtons)
                    mostButtons = b.size();
            }
            PressInt theoreticalMin { (maxJoltage > joltageSum / mostButtons ? maxJoltage : (joltageSum / mostButtons % 2 == 0 ? joltageSum / mostButtons : joltageSum / mostButtons + 1)) };

            // solve properly
            PressInt min { solveIteratively(joltages, schematic, theoreticalMin) };

            total += min;
        }

        return total;
    }
}

#endif // AOC10B_H
