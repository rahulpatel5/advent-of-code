#ifndef AOC6A_H
#define AOC6A_H

#include <string_view>
#include <array>
#include <vector>
#include <set>
#include <stdexcept>

/*
find starting position
find initial direction
create set/vector to collect position, including initial
check next position. If this is an 'obstacle', change direction
have function to turn right
check if position is on outside (first/last row/column), which signals end
count distinct positions
*/

namespace aoc6a
{
    template <std::size_t N>
    std::pair<int, int> findStartPosition(std::array<std::string_view, N> lines)
    {
        for (size_t i{0}; i < lines.size(); ++i)
        {
            for (size_t j{0}; j < lines.at(i).size(); ++j)
            {
                char c {lines.at(i).at(j)};
                if ((c == '^') || (c == '>') || (c == 'v') || (c == '<'))
                    return {j, i};
            }
        }

        throw std::invalid_argument("did not find a valid starting position");
    }

    std::pair<int, int> getStartDirection(char c)
    {
        switch (c)
        {
        case '^':   return { 0, -1};
        case '>':   return { 1,  0};
        case 'v':   return { 0,  1};
        case '<':   return {-1,  0};
        default:    throw std::invalid_argument("received invalid starting direction");
        }
    }

    std::pair<int, int> getNextPosition(std::pair<int, int> currentPosition, std::pair<int, int> direction)
    {
        return {currentPosition.first + direction.first, currentPosition.second + direction.second};
    }

    template <std::size_t N>
    bool isLeavingArea(std::pair<int, int> position, std::array<std::string_view, N> lines)
    {
        return (position.first == 0) || (position.second == 0) || (position.first == lines.at(0).size() - 1) || (position.second == lines.size() - 1);
    }

    template <std::size_t N>
    bool isNextPositionObstacle(std::pair<int, int> position, std::array<std::string_view, N> lines)
    {
        return lines.at(position.second).at(position.first) == '#';
    }

    std::pair<int, int> turnRight(std::pair<int, int> currentDirection)
    {
        if ((currentDirection.first == 0) && (currentDirection.second == -1))
            return { 1,  0};
        else if ((currentDirection.first == 1) && (currentDirection.second == 0))
            return { 0,  1};
        else if ((currentDirection.first == 0) && (currentDirection.second == 1))
            return {-1,  0};
        else if ((currentDirection.first == -1) && (currentDirection.second == 0))
            return { 0, -1};
        else
            throw std::invalid_argument("received invalid previous direction");
    }

    template <std::size_t N>
    int parseAndCountDistinctPositions(std::array<std::string_view, N> lines)
    {
        std::set<std::pair<int, int>> distinctPositions {};

        std::pair<int, int> position {findStartPosition(lines)};
        std::pair<int, int> direction {getStartDirection(lines.at(position.second).at(position.first))};
        distinctPositions.insert(position);

        while (!isLeavingArea(position, lines))
        {
            std::pair<int, int> nextPosition {getNextPosition(position, direction)};

            if (isNextPositionObstacle(nextPosition, lines))
                direction = turnRight(direction);
            else
                position = nextPosition;

            distinctPositions.insert(position);
        }

        return distinctPositions.size();
    }
}

#endif // AOC6A_H
