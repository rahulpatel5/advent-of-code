#ifndef AOC19B_H
#define AOC19B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <ranges>
#include <numeric>
#include <cassert>

/*
IDENTIFY all ending points
ITERATE over input
    FIND longest section that can be matched
    REPLACE section
    RETURN to start of input
COUNT (fewest) replacements required

failed ideas:
    split input at break points where there are molecules that can only be added using "e". Fails because input isn't split in this way. e.g. the first molecule isn't from "e" so extra steps needed
*/

namespace aoc19b
{
    using StepInt = int;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Molecule = std::string;
    using Input = std::string;
    using Molecules = std::vector<Molecule>;
    using Replacements = std::map<Molecule, Molecules>;
    using Formations = std::map<Molecule, Molecules>;
    using MaxLength = int;

    using UniqueMol = std::set<char>;

    template <std::size_t N>
    void readInput(const Lines<N>& lines, Replacements& replacements, Formations& formations, Input& input)
    {
        for (std::string_view line : lines)
        {
            if (line.empty())
                continue;
            
            std::string row {std::string(line)};

            if (row.find("=>") == std::string::npos)
            {
                input = row;
                continue;
            }

            size_t firstSpace {row.find(' ')};
            size_t secondSpace {row.find(' ', firstSpace + 1)};
            Molecule firstMol {row.substr(0, firstSpace)};
            Molecule secondMol {row.substr(secondSpace + 1)};

            replacements[firstMol].push_back(secondMol);
            formations[secondMol].push_back(firstMol);
            // check that there's only ever one option for formations
            assert(formations.at(secondMol).size() == 1 && "Expected only one option for generating molecule, but there's more than one.\n");
        }
    }

    MaxLength getMaxLength(const Formations& formations)
    {
        auto keys {std::views::keys(formations)};
        return std::accumulate(keys.begin(), keys.end(), 0, [](int max, std::string s) {
            return (s.size() > max) ? s.size() : max;
        });
    }

    Molecules orderByLongest(const Formations& formations, MaxLength maxL)
    {
        Molecules longest {};
        auto keys {std::views::keys(formations)};
        for (int n{maxL}; n > 0; --n)
        {
            for (auto key : keys)
            {
                if (key.size() == n)
                {
                    longest.push_back(key);
                }
            }
        }
        return longest;
    }

    bool checkSuccess(const Input& input)
    {
        UniqueMol unique {input.begin(), input.end()};
        return (unique.size() == 1 && unique.find('e') != unique.end());
    }

    StepInt replaceLongestMolecules(const Input& input, const Molecules& longest, const Replacements& replacements, const Formations& formations, MaxLength maxL)
    {
        StepInt steps {0};
        Input revInput {input};

        while (!checkSuccess(revInput))
        {
            for (const Molecule& mol : longest)
            {
                if (revInput.find(mol) != std::string::npos)
                {
                    // formations has only 1 option (previous check)
                    revInput.replace(revInput.find(mol), mol.size(), formations.at(mol).at(0));
                    ++steps;
                    // let's re-start the loop
                    break;
                }
            }
        }

        return steps;
    }

    template <std::size_t L>
    StepInt countFewestSteps(const Lines<L>& lines)
    {
        Replacements replacements {};
        Formations formations {};
        Input input {};
        readInput<L>(lines, replacements, formations, input);
        MaxLength maxMol {getMaxLength(formations)};

        Molecules orderLongestFirst {orderByLongest(formations, maxMol)};
        return replaceLongestMolecules(input, orderLongestFirst, replacements, formations, maxMol);
    }
}

#endif // AOC19B_H
