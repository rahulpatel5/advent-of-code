#ifndef AOC17B_H
#define AOC17B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

/*
CREATE all combinations
IF combination sums to target
    THEN pass test
    * IF combination length smaller than any other
        * THEN set minimum combination length
        * AND set counter to 1
    * ELSE if combination is same length as current minimum
        * THEN increase current counter by 1
    MOVE to next combination
ELSE
    THEN fail test
    MOVE to next combination

* at start of line shows change to the part 1 solution
*/

namespace aoc17b
{
    using ComboInt = int;
    using Container = int;
    using Containers = std::vector<Container>;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    template <std::size_t N>
    Containers getContainers(const Lines<N>& lines)
    {
        Containers containers {};
        for (std::string_view row : lines)
        {
            containers.push_back(std::stoi(std::string(row)));
        }
        return containers;
    }

    template <int target>
    ComboInt recursiveCombos(size_t start, const Containers& containers, Containers& current, ComboInt& minCombo, ComboInt& minCounter)
    {
        for (size_t i{start}; i < containers.size(); ++i)
        {
            current.push_back(containers.at(i));

            if (std::accumulate(current.begin(), current.end(), 0) == target)
            {
                if (current.size() < minCombo || minCombo == 0)
                {
                    minCombo = current.size();
                    minCounter = 1;
                }
                else if (current.size() == minCombo)
                    ++minCounter;
            }

            recursiveCombos<target>(i + 1, containers, current, minCombo, minCounter);

            current.pop_back();
        }
        return minCombo;
    }

    template <int target>
    ComboInt validCombos(const Containers& containers)
    {
        ComboInt minCombo {};
        ComboInt minCounter {};

        size_t startIndex {0};
        Containers current {};
        minCombo = recursiveCombos<target>(startIndex, containers, current, minCombo, minCounter);

        return minCounter;
    }

    template <std::size_t L, int target>
    ComboInt getMinCombo(const Lines<L>& lines)
    {
        Containers containers {getContainers<L>(lines)};
        // helpful to sort this in descending order
        std::sort(containers.begin(), containers.end(), std::greater<Container>());

        return validCombos<target>(containers);
    }
}

#endif // AOC17B_H
