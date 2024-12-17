#ifndef DIRECTION_H
#define DIRECTION_H

#include <array>
#include <stdexcept>

namespace Direction
{
    constexpr std::pair<int, int> north { 0, -1};
    constexpr std::pair<int, int> east  { 1,  0};
    constexpr std::pair<int, int> south { 0,  1};
    constexpr std::pair<int, int> west  {-1,  0};

    constexpr std::array<std::pair<int, int>, 4> allDirections {north, east, south, west};
    
    constexpr std::pair<int, int> turnClockwise(std::pair<int, int> move)
    {
        if (move == north)
            return east;
        else if (move == east)
            return south;
        else if (move == south)
            return west;
        else if (move == west)
            return north;
        else
            throw std::invalid_argument("Given non-valid initial direction.\n");
    }
    
    constexpr std::pair<int, int> turnAntiClockwise(std::pair<int, int> move)
    {
        if (move == north)
            return west;
        else if (move == east)
            return north;
        else if (move == south)
            return east;
        else if (move == west)
            return south;
        else
            throw std::invalid_argument("Given non-valid initial direction.\n");
    }
}

#endif // DIRECTION_H
