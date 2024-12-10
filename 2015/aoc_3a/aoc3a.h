#ifndef AOC3A_H
#define AOC3A_H

#include <iostream>
#include <map>

namespace aoc3a
{
    void moveDirection(char dir, int& x, int& y)
    {
        switch(dir)
        {
        case '^':
            ++y;
            return;
        case '>':
            ++x;
            return;
        case 'v':
            --y;
            return;
        case '<':
            --x;
            return;
        default:
            std::cout << "Not a direction.\n";
            return;
        }
    }

    void addHouse(std::map<std::pair<int,int>, int>& m, int x, int y)
    {
        m[{x, y}] += 1;
    }

    int housesVisited(std::string_view input)
    {
        int x{0};
        int y{0};
        std::map<std::pair<int,int>, int> houses{};
        addHouse(houses, x, y);
        for (char c : input)
        {
            moveDirection(c, x, y);
            addHouse(houses, x, y);
        }
        return houses.size();
    }
}

#endif
