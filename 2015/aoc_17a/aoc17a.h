#ifndef AOC17A_H
#define AOC17A_H

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
    COUNT as valid combination
    MOVE to next combination
ELSE
    THEN fail test
    MOVE to next combination
*/

namespace aoc17a
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
    ComboInt recursiveCombos(size_t start, const Containers& containers, Containers& current, ComboInt& adder)
    {
        for (size_t i{start}; i < containers.size(); ++i)
        {
            current.push_back(containers.at(i));

            if (std::accumulate(current.begin(), current.end(), 0) == target)
            {
                ++adder;
            }

            recursiveCombos<target>(i + 1, containers, current, adder);

            current.pop_back();
        }
        return adder;
    }

    template <int target>
    ComboInt validCombos(const Containers& containers)
    {
        ComboInt valid {};

        size_t startIndex {0};
        Containers current {};
        ComboInt adder {0};
        valid += recursiveCombos<target>(startIndex, containers, current, adder);

        return valid;
    }

    template <std::size_t L, int target>
    ComboInt countCombos(const Lines<L>& lines)
    {
        Containers containers {getContainers<L>(lines)};
        // helpful to sort this in descending order
        std::sort(containers.begin(), containers.end(), std::greater<Container>());

        return validCombos<target>(containers);
    }
}

#endif // AOC17A_H
