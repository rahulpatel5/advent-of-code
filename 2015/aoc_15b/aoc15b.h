#ifndef AOC15B_H
#define AOC15B_H

#include <array>
#include <string_view>
#include <string>
#include <map>
#include <vector>
#include <numeric>
#include <cassert>
#include <algorithm>
#include <stdexcept>

/*
re-use solution to part 1
there should be a limited number of combos that have the constraint
try collecting all those first and seeing how many there are?
*/

namespace aoc15b
{
    using Score = long long;

    using Property = int;
    using Capacity = Property;
    using Durability = Property;
    using Flavour = Property;
    using Texture = Property;
    using Calories = Property;

    using Ingredient = int;
    constexpr int NumberProperties {4};
    using IngredientMap = std::map<Ingredient, std::vector<Property>>;
    using CaloriesList = std::vector<Calories>;

    using Spoon = int;
    using Spoons = std::vector<Spoon>;
    using SpoonPair = std::pair<Spoon, Spoon>;
    using SpoonPairs = std::vector<SpoonPair>;
    using SpoonCombos = std::vector<Spoons>;
    constexpr int MaxSpoons {100};
    // min and max number of spoons for each ingredient
    using SpoonLimit = std::pair<Spoon, Spoon>;
    using SpoonLimits = std::vector<SpoonLimit>;
    using PropertyTotals = std::vector<Property>;

    template <std::size_t N>
    int getProperties(const std::array<std::string_view, N>& lines, IngredientMap& ingredients, CaloriesList& calList)
    {
        int ingredientIndex {0};

        for (std::string_view row : lines)
        {
            std::string line {std::string(row)};

            size_t firstSpace {line.find(' ')};
            size_t capacitySpace {line.find(' ', firstSpace + 1)};
            size_t capacityComma {line.find(',')};
            Capacity capacity {std::stoi(line.substr(capacitySpace + 1, capacityComma - (capacitySpace + 1)))};

            size_t durabilitySpace {line.find(' ', capacityComma + 2)};
            size_t durabilityComma {line.find(',', capacityComma + 1)};
            Durability durability {std::stoi(line.substr(durabilitySpace + 1, durabilityComma - (durabilitySpace + 1)))};

            size_t flavourSpace {line.find(' ', durabilityComma + 2)};
            size_t flavourComma {line.find(',', durabilityComma + 1)};
            Flavour flavour {std::stoi(line.substr(flavourSpace + 1, flavourComma - (flavourSpace + 1)))};

            size_t textureSpace {line.find(' ', flavourComma + 2)};
            size_t textureComma {line.find(',', flavourComma + 1)};
            Texture texture {std::stoi(line.substr(textureSpace + 1, textureComma - (textureSpace + 1)))};

            size_t caloriesSpace {line.find(' ', textureComma + 2)};
            size_t caloriesComma {line.find(',', textureComma + 1)};
            Calories calories {std::stoi(line.substr(caloriesSpace + 1, caloriesComma - (caloriesSpace + 1)))};

            ingredients[0].push_back(capacity);
            ingredients[1].push_back(durability);
            ingredients[2].push_back(flavour);
            ingredients[3].push_back(texture);

            calList.push_back(calories);

            ++ingredientIndex;
        }

        return ingredientIndex;
    }

    SpoonLimits getLimits(int numberIngredients, const IngredientMap& ingredients)
    {
        SpoonLimits limits {};
        Spoon absMinimum {1};
        Spoon absMaximum {MaxSpoons - (numberIngredients - 1)};
        for (int i{0}; i < numberIngredients; ++i)
        {
            limits.push_back({absMinimum, absMaximum});
        }

        PropertyTotals totals {};
        for (int i{0}; i < numberIngredients; ++i)
        {
            for (int j{0}; j < NumberProperties; ++j)
            {
                if (i == 0)
                    totals.push_back(std::abs(ingredients.at(j).at(i)));
                else
                    totals[j] += std::abs(ingredients.at(j).at(i));
            }
        }

        for (int i{0}; i < numberIngredients; ++i)
        {
            for (int j{0}; j < NumberProperties; ++j)
            {
                Spoon lim {100 - 100 * std::abs(ingredients.at(j).at(i)) / totals.at(j) - 1};

                if (ingredients.at(j).at(i) < 0 && limits.at(i).second > lim)
                {
                    limits[i].second = lim;
                }
            }
        }

        return limits;
    }

    Score getScore(const IngredientMap& ingredients, const Spoons& combination)
    {
        Score score {1};

        for (int j{0}; j < NumberProperties; ++j)
        {
            // std::transform_reduce can be run parallel-ly?
            Score ingredScore {std::inner_product(ingredients.at(j).begin(), ingredients.at(j).end(), combination.begin(), 0)};
            
            if (ingredScore <= 0)
                return 0;
            score *= ingredScore;
        }

        return score;
    }

    template <int target>
    bool meetTarget(const CaloriesList& calList, const Spoons& combo)
    {
        return (std::inner_product(calList.begin(), calList.end(), combo.begin(), 0) == target);
    }

    Score optimiseSpoons(const SpoonCombos& comboList, const SpoonLimits& limits, const IngredientMap& ingredients)
    {
        Score maxScore {};

        for (const Spoons& combo : comboList)
        {
            Score newScore {getScore(ingredients, combo)};
            if (newScore > maxScore)
            {
                maxScore = newScore;
            }
        }

        return maxScore;
    }

    SpoonPairs getPairSolution(Spoon sum, Calories pen, Calories fin, int newTarget)
    {
        Spoon first {1};
        Spoon second {sum - first};
        int guess1 {first * pen + second * fin};
        
        first += 1;
        second = sum - first;
        int guess2 {first * pen + second * fin};
        
        if (guess1 == guess2 && guess1 == newTarget)
        {
            SpoonPairs allPairs {};
            for (Spoon sp{1}; sp <= sum - 1; ++sp)
            {
                allPairs.push_back(std::make_pair(sp, sum - sp));
            }
            return allPairs;
        }
        else if (guess1 == newTarget)
        {
            return {std::make_pair(1, sum - 1)};
        }
        else if (guess2 == newTarget)
        {
            return {std::make_pair(2, sum - 2)};
        }
        
        int diff {(guess1 > guess2) ? (guess1 - guess2) : (guess2 - guess1)};
        // if there's no solution
        if (diff == 0 || (newTarget - guess1) % diff != 0)
            return {std::make_pair(0, 0)};
        
        first = ((newTarget - guess1) / diff) + 1;
        second = sum - first;
        if (first <= 0 || second <= 0)
            return {std::make_pair(0, 0)};
        assert(first * pen + second * fin == newTarget && "Did not find a pair solution when this was expected.\n");
        return {std::make_pair(first, second)};
    }

    template <int target>
    SpoonCombos recursiveGetCombos(SpoonCombos& combos, Spoons& collection, const SpoonLimits& limits, const CaloriesList& calList, Spoons currentSpoons, int index)
    {
        if (currentSpoons.front() >= limits.at(0).second)
        {
            return combos;
        }
        
        if (index < calList.size() - 2)
        {
            collection.push_back(currentSpoons.at(index));
            return recursiveGetCombos<target>(combos, collection, limits, calList, currentSpoons, index + 1);
        }
        else
        {
            Spoon lastSpoons {MaxSpoons - std::accumulate(collection.begin(), collection.end(), 0)};

            int newTarget {target - std::inner_product(collection.begin(), collection.end(), calList.begin(), 0)};

            SpoonPairs pairs {(lastSpoons > 1 && lastSpoons <= MaxSpoons - 2) ? getPairSolution(lastSpoons, calList[calList.size() - 2], calList[calList.size() - 1], newTarget) : SpoonPairs{std::make_pair(0, 0)}};

            for (SpoonPair sp : pairs)
            {
                Spoons iterColl {collection};
                iterColl.push_back(sp.first);
                iterColl.push_back(sp.second);

                if (sp != std::make_pair(0, 0) && meetTarget<target>(calList, iterColl))
                {
                    combos.push_back(iterColl);
                }
            }

            Spoons newColl {};
            bool addedIteration {false};
            for (int n{static_cast<int>(limits.size() - 3)}; n >= 0; --n)
            {
                if (addedIteration)
                {
                    /* intentionally empty */
                }
                else if (currentSpoons.at(n) >= limits.at(n).second || std::accumulate(newColl.begin(), newColl.end(), 0) + std::accumulate(currentSpoons.begin(), currentSpoons.begin() + n + 1, 0) >= MaxSpoons - 2)
                {
                    currentSpoons[n] = limits.at(n).first;
                }
                else if (currentSpoons.at(n) < limits.at(n).second)
                {
                    currentSpoons[n] += 1;
                    index = n + 1;
                    addedIteration = true;
                }
                else
                {
                    throw std::out_of_range("Reached condition that shouldn't have been reached.\n");
                }
                newColl.push_back(currentSpoons.at(n));
            }
            collection = std::move(newColl);
            std::reverse(collection.begin(), collection.end());
            assert(collection.size() <= limits.size() - 2 && "Number of carry-over elements should be less than the max.\n");

            return recursiveGetCombos<target>(combos, collection, limits, calList, currentSpoons, index);
        }
    }

    template <int target>
    Score maxScore(int numberIngredients, const IngredientMap& ingredients, const CaloriesList& calList)
    {
        Score max {};

        SpoonLimits limits {getLimits(numberIngredients, ingredients)};

        SpoonCombos targetCombos {};
        Spoons currentSpoons {};
        for (int n{0}; n < limits.size(); ++n)
            currentSpoons.push_back(limits.at(n).first);
        Spoons collection {};
        recursiveGetCombos<target>(targetCombos, collection, limits, calList, currentSpoons, 0);

        return optimiseSpoons(targetCombos, limits, ingredients);
    }

    template <std::size_t N, int target>
    Score getCookieScore(const std::array<std::string_view, N>& lines)
    {
        IngredientMap ingredients {};
        CaloriesList calList {};
        int numberIngredients {getProperties<N>(lines, ingredients, calList)};

        return maxScore<target>(numberIngredients, ingredients, calList);
    }
}

#endif // AOC15B_H
