#ifndef AOC19A_H
#define AOC19A_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <ranges>
#include <numeric>
#include <algorithm>

/*
IDENTIFY all starting points
ITERATE over starting molecule
IF starting point found
    THEN store replaced versions
    MOVE onto finding next starting point for original molecule
COUNT unique replaced versions
*/

namespace aoc19a
{
    using MolInt = int;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Molecule = std::string;
    using Input = std::string;
    using Molecules = std::vector<Molecule>;
    using Replacements = std::map<Molecule, Molecules>;
    using UniqueMolecules = std::set<Molecule>;
    using MaxLength = int;

    template <std::size_t N>
    Replacements getReplacements(const Lines<N>& lines)
    {
        Replacements replacements {};
        for (std::string_view line : lines)
        {
            if (line.find("=>") == std::string_view::npos)
                continue;

            std::string row {std::string(line)};
            size_t firstSpace {row.find(' ')};
            size_t secondSpace {row.find(' ', firstSpace + 1)};
            Molecule firstMol {row.substr(0, firstSpace)};
            Molecule secondMol {row.substr(secondSpace + 1)};

            replacements[firstMol].push_back(secondMol);
        }
        return replacements;
    }

    template <std::size_t N>
    Input getInput(const Lines<N>& lines)
    {
        for (std::string_view line : lines)
        {
            if (line.empty() || line.find("=>") != std::string_view::npos)
                continue;
            
            return std::string(line);
        }
        throw std::out_of_range("Did not find valid input.\n");
    }

    MaxLength getMaxKeyLength(const Replacements& replacements)
    {
        auto keys {std::views::keys(replacements)};
        return std::accumulate(keys.begin(), keys.end(), 0, [](int max, std::string s) {
            return (s.size() > max) ? s.size() : max;
        });
    }

    MolInt getDistinctMolecules(const Replacements& replacements, const Input& input, MaxLength maxL)
    {
        UniqueMolecules unique {};
        size_t index {0};
        while (index < input.size())
        {
            size_t prevIndex {index};
            for (int n{maxL}; n > 0; --n)
            {
                Molecule starter {input.substr(index, n)};
                if (replacements.find(starter) != replacements.end())
                {
                    for (Molecule m : replacements.at(starter))
                    {
                        Input copyInput {input};
                        copyInput.replace(index, n, m);
                        unique.insert(copyInput);
                    }
                    index += n;
                    break;
                }
            }
            if (index == prevIndex)
                ++index;
        }
        return unique.size();
    }

    template <std::size_t L>
    MolInt countDistinctMolecules(const Lines<L>& lines)
    {
        Replacements replacements {getReplacements<L>(lines)};
        Input input {getInput<L>(lines)};
        MaxLength maxMol {getMaxKeyLength(replacements)};

        return getDistinctMolecules(replacements, input, maxMol);
    }
}

#endif // AOC19A_H
