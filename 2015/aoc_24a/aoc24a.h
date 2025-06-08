#ifndef AOC24A_H
#define AOC24A_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <functional>
#include <set>

/*
FIND total weight / 3, for target weight of each group
ITERATE over combinations that meet same weight criteria
IF number of items in group 1 is smallest (and weight criteria met)
    COLLECT the item combination
    MOVE to the next combination
ELSE IF number of items matches the smallest
    ADD the combination to the existing collection
    MOVE to the next combination
FIND the min quantum entanglement of the smallest combination(s)

order combinations so heaviest items are in the min item group
could use a recursive function to create the combinations
*/

namespace aoc24a
{
    using Quantum = long long;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Present = int;
    using Presents = std::vector<Present>;
    using Weight = long long;
    using UniquePresents = std::set<Present>;

    template <std::size_t N>
    Presents getPresents(const Lines<N>& lines)
    {
        Presents presents {};
        for (std::string_view row : lines)
        {
            if (row.empty())
                continue;
            presents.push_back(std::stoi(std::string(row)));
        }
        return presents;
    }

    Quantum getEntanglement(const Presents& combo)
    {
        return std::accumulate(combo.begin(), combo.end(), 1ll, std::multiplies<Quantum>());
    }

    bool recursiveDoesSumToTarget(const Presents& remainingPresents, Presents combo, size_t index, Weight target, bool& doesSum)
    {
        for (size_t i{index}; i < remainingPresents.size(); ++i)
        {
            combo.push_back(remainingPresents.at(i));
            Weight sum {std::accumulate(combo.begin(), combo.end(), 0)};

            if (sum == target)
            {
                doesSum = true;
                return doesSum;
            }

            if (!doesSum)
                recursiveDoesSumToTarget(remainingPresents, combo, i + 1, target, doesSum);

            combo.pop_back();
        }
        return doesSum;
    }

    bool doesReachTarget(const Presents& presents, const Presents& combo, Weight target)
    {
        Presents remaining {};
        std::set_difference(presents.begin(), presents.end(), combo.begin(), combo.end(), std::inserter(remaining, remaining.begin()));
        Presents newCombo {};
        size_t index {0};
        bool doesSum {false};
        return recursiveDoesSumToTarget(remaining, newCombo, index, target, doesSum);
    }

    Quantum findLeastQuantum(const Presents& presents, Presents combo, size_t index, Weight target, size_t& minValidComboSz, Quantum& minQuantum)
    {
        for (size_t i{index}; i < presents.size(); ++i)
        {
            if (minValidComboSz > 0 && combo.size() > minValidComboSz)
                break;

            combo.push_back(presents.at(i));
            Weight sum {std::accumulate(combo.begin(), combo.end(), 0)};

            if (sum == target && (minValidComboSz == 0 || combo.size() <= minValidComboSz) && (minQuantum == 0 || combo.size() < minValidComboSz || (combo.size() == minValidComboSz && getEntanglement(combo) < minQuantum)) && doesReachTarget(presents, combo, target))
            {
                minValidComboSz = combo.size();
                minQuantum = getEntanglement(combo);
            }

            else if (sum < target && (minValidComboSz == 0 || combo.size() < minValidComboSz))
                findLeastQuantum(presents, combo, i + 1, target, minValidComboSz, minQuantum);

            combo.pop_back();
        }
        return minQuantum;
    }

    Quantum getMinQuantum(const Presents& presents, Weight target)
    {
        Presents combo {};
        size_t index {0};
        size_t minValidComboSize {0};
        Quantum currQuantum {0};
        Quantum minQuantum {findLeastQuantum(presents, combo, index, target, minValidComboSize, currQuantum)};
        return minQuantum;
    }

    template <std::size_t L, int groups>
    Quantum getPresentEntanglement(const Lines<L>& lines)
    {
        Presents presents {getPresents<L>(lines)};
        Weight totalWeight {0};
        for (Present p : presents)
            totalWeight += p;
        assert(totalWeight % groups == 0 && "Expected total weight to be divisible by the number of groups.\n");
        Weight target {totalWeight / groups};

        // we assume later that presents has unique elements
        UniquePresents unique (presents.begin(), presents.end());
        assert(presents.size() == unique.size() && "The elements in presents are not unique.\n");

        return getMinQuantum(presents, target);
    }
}

#endif // AOC24A_H
