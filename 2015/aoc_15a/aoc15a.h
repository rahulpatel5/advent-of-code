#ifndef AOC15A_H
#define AOC15A_H

#include <array>
#include <string_view>
#include <string>
#include <map>
#include <vector>
#include <numeric>
#include <cassert>

/*
get properties for each ingredient
set up formulas for each ingredient and property
find solution for max product of ingredients & properties

how can we solve this most easily?
can we numerically solve this?
does this assume that a continuous function describes the formulas?
is this assumption valid?

we're multiplying each ingredient (and summing each property)
so we're solving an equation of that many dimensions
would it work to find the max spoons for each ingredient in turn?
*/

namespace aoc15a
{
    using Score = long long;

    using Property = int;
    using Capacity = Property;
    using Durability = Property;
    using Flavour = Property;
    using Texture = Property;
    // ignoring calories for now
    using Calories = Property;

    using Ingredient = int;
    constexpr int NumberProperties {4};
    using IngredientMap = std::map<Ingredient, std::vector<Property>>;

    using Spoon = int;
    using Spoons = std::vector<Spoon>;
    constexpr int MaxSpoons {100};
    // min and max number of spoons for each ingredient
    using SpoonLimit = std::pair<Spoon, Spoon>;
    using SpoonLimits = std::vector<SpoonLimit>;
    using PropertyTotals = std::vector<Property>;

    template <std::size_t N>
    int getProperties(const std::array<std::string_view, N>& lines, IngredientMap& ingredients)
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

            // ignoring calories for now
            // size_t caloriesSpace {line.find(' ', textureComma + 2)};
            // size_t caloriesComma {line.find(',', textureComma + 1)};
            // Calories calories {std::stoi(line.substr(caloriesSpace + 1, caloriesComma - (caloriesSpace + 1)))};

            ingredients[0].push_back(capacity);
            ingredients[1].push_back(durability);
            ingredients[2].push_back(flavour);
            ingredients[3].push_back(texture);
            // ingredients[ingredientIndex] = {capacity, durability, flavour, texture};
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

    Spoons optimiseSpoons(const Spoons& combo, const SpoonLimits& limits, const IngredientMap& ingredients)
    {
        Score comboScore {getScore(ingredients, combo)};
        Score newScore {comboScore};
        Score maxScore {comboScore};
        Spoons nextCombo {combo};

        do
        {
            comboScore = newScore;
            Spoons currCombo {nextCombo};

            for (int i{0}; i < combo.size(); ++i)
            {
                for (int j{0}; j < combo.size(); ++j)
                {
                    if (i == j)
                        continue;

                    Spoons newCombo {currCombo};
                    newCombo[i] += 1;
                    newCombo[j] -= 1;

                    assert(std::accumulate(newCombo.begin(), newCombo.end(), 0) == MaxSpoons && "Total number of spoons is not the fixed amount expected.\n");
                    
                    newScore = getScore(ingredients, newCombo);
                    if (newScore > maxScore)
                    {
                        maxScore = newScore;
                        nextCombo = newCombo;
                    }
                }
            }
        }
        while (comboScore == 0 || comboScore != newScore);

        return nextCombo;
    }

    Score maxScore(int numberIngredients, const IngredientMap& ingredients)
    {
        Score max {};

        SpoonLimits limits {getLimits(numberIngredients, ingredients)};

        Spoons combo {};
        // initially have all spoons equal
        Spoon equal {MaxSpoons / numberIngredients};
        assert(MaxSpoons % numberIngredients == 0 && "Total number of spoons is not divisible by the number of ingredients, as was expected.\n");
        for (int _n{0}; _n < numberIngredients; ++_n)
            combo.push_back(equal);
        
        Spoons optimised {optimiseSpoons(combo, limits, ingredients)};

        return getScore(ingredients, optimised);
    }

    template <std::size_t N>
    Score getCookieScore(const std::array<std::string_view, N>& lines)
    {
        IngredientMap ingredients {};
        int numberIngredients {getProperties<N>(lines, ingredients)};

        return maxScore(numberIngredients, ingredients);
    }
}

#endif // AOC15A_H
