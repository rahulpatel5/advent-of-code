#ifndef AOC3B_H
#define AOC3B_H

#include "../aoc_3a/aoc3a.h"
#include <map>

namespace aoc3b
{
    int housesVisited(std::string_view input)
    {
        int xS{0}; // S for regular Santa
        int yS{0};
        int xR{0}; // S for robo-Santa
        int yR{0};
        std::map<std::pair<int,int>, int> houses{};
        aoc3a::addHouse(houses, xS, yS);
        aoc3a::addHouse(houses, xR, yR);
        for (int i{0}; i < input.length(); ++i)
        {
            if (i % 2 == 0)
            {
                aoc3a::moveDirection(input[i], xS, yS);
                aoc3a::addHouse(houses, xS, yS);
            }
            else
            {
                aoc3a::moveDirection(input[i], xR, yR);
                aoc3a::addHouse(houses, xR, yR);
            }
        }
        return houses.size();
    }
}

#endif
