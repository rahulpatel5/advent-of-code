#ifndef AOC13B_H
#define AOC13B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <ranges>

/*
re-use solution to part 1
add 0 happiness relationships
*/

namespace aoc13b
{
    using HapInt = long long;
    using HapUnit = int;

    using PersonName = std::string;
    using Person = int;
    using PersonMap = std::map<PersonName, Person>;
    using PeoplePair = std::pair<Person, Person>;
    using HappyMap = std::map<std::pair<Person, Person>, HapUnit>;

    // order of people on table. Doesn't connect first and last
    using TableOrder = std::vector<Person>;

    template <std::size_t N>
    int processInput(const std::array<std::string_view, N>& lines, HappyMap& happyMap)
    {
        Person personIndex {0};
        PersonMap people {};

        for (std::string_view line : lines)
        {
            // collecting names
            size_t firstSpace {line.find(' ')};
            PersonName firstName {line.substr(0, firstSpace)};
            if (people.find(firstName) == people.end())
            {
                people[firstName] = personIndex;
                ++personIndex;
            }

            size_t lastSpace {line.find_last_of(' ')};
            size_t fullStop {line.find('.')};
            PersonName lastName {line.substr(lastSpace + 1, fullStop - (lastSpace + 1))};
            if (people.find(lastName) == people.end())
            {
                people[lastName] = personIndex;
                ++personIndex;
            }

            // collecting happiness units
            size_t secondSpace {line.find(' ', firstSpace + 1)};
            size_t thirdSpace {line.find(' ', secondSpace + 1)};
            size_t fourthSpace {line.find(' ', thirdSpace + 1)};

            HapUnit happ {std::stoi(std::string(line.substr(thirdSpace + 1, fourthSpace - (thirdSpace + 1))))};
            PeoplePair pair {people.at(firstName), people.at(lastName)};

            bool isPositive {line.substr(secondSpace + 1, thirdSpace - (secondSpace + 1)) == "gain"};
            if (!isPositive)
                happ = -happ;

            happyMap[pair] = happ;
        }

        return personIndex;
    }

    HapInt maxHappiness(int n, const HappyMap& happyMap, int maxHappy)
    {
        HapInt max {};

        TableOrder order {};
        for (int i{0}; i < n; ++i)
            order.push_back(i);

        do
        {
            HapInt happy {0};
            for (size_t n{0}; n < order.size(); ++n)
            {
                PeoplePair pair {};
                // pair on one side
                if (n == 0)
                    pair = {order.at(n), order.back()};
                else
                    pair = {order.at(n), order.at(n - 1)};
                happy += happyMap.at(pair);

                // pair on other side
                if (n == order.size() - 1)
                    pair = {order.at(n), order.front()};
                else
                    pair = {order.at(n), order.at(n + 1)};
                happy += happyMap.at(pair);

                if ((order.size() - 1 - n) * maxHappy + happy <= max)
                    break;
            }
            if (happy > max)
                max = happy;
        }
        while (std::next_permutation(order.begin(), order.end()));

        return max;
    }

    template <std::size_t N>
    HapInt getHappy(const std::array<std::string_view, N>& lines)
    {
        HappyMap happyMap {};
        int numberPeople {processInput<N>(lines, happyMap)};

        auto values {std::views::values(happyMap)};
        int maxHappy {std::ranges::max(values)};

        // add extra person (me, apparently)
        for (int i{0}; i < numberPeople; ++i)
        {
            happyMap[{i, numberPeople}] = 0;
            happyMap[{numberPeople, i}] = 0;
        }
        ++numberPeople;

        return maxHappiness(numberPeople, happyMap, maxHappy);
    }
}

#endif // AOC13B_H
