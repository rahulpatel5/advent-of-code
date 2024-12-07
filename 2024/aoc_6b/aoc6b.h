#ifndef AOC6B_H
#define AOC6B_H

#include <string_view>
#include <array>
#include <vector>
#include <set>
#include <stdexcept>
#include <string>

/*
INCORRECT:  re-use solution from part a to get distinct values
            this is the original route, so new obstacle only matters if it's on this path
            iterate through original route, adding one obstacle for each version
            check if it loops

iterate through whole map, adding one obstacle for each version
re-use solution from part a to test if new map loops
*/

namespace aoc6b
{
    template <std::size_t N>
    std::pair<size_t, size_t> findStartPosition(const std::array<std::string_view, N>& lines)
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

    std::pair<size_t, size_t> getStartDirection(char c)
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

    std::pair<size_t, size_t> getNextPosition(std::pair<size_t, size_t> currentPosition, std::pair<size_t, size_t> direction)
    {
        return {currentPosition.first + direction.first, currentPosition.second + direction.second};
    }

    template <std::size_t N>
    bool isLeavingArea(std::pair<size_t, size_t> position, const std::array<std::string_view, N>& lines)
    {
        return (position.first == 0) || (position.second == 0) || (position.first == lines.at(0).size() - 1) || (position.second == lines.size() - 1);
    }

    template <std::size_t N>
    bool isNextPositionObstacle(std::pair<size_t, size_t> position, const std::array<std::string_view, N>& lines)
    {
        return lines.at(position.second).at(position.first) == '#';
    }

    std::pair<size_t, size_t> turnRight(std::pair<size_t, size_t> currentDirection)
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
    std::set<std::pair<size_t, size_t>> getOriginalDistinctPositions(const std::array<std::string_view, N>& lines)
    {
        std::set<std::pair<size_t, size_t>> distinctPositions;

        std::pair<size_t, size_t> position {findStartPosition(lines)};
        std::pair<size_t, size_t> direction {getStartDirection(lines.at(position.second).at(position.first))};
        distinctPositions.insert(position);

        while (!isLeavingArea(position, lines))
        {
            std::pair<size_t, size_t> nextPosition {getNextPosition(position, direction)};

            if (isNextPositionObstacle(nextPosition, lines))
                direction = turnRight(direction);
            else
                position = nextPosition;

            distinctPositions.insert(position);
        }

        return distinctPositions;
    }
    
    template <std::size_t N>
    std::string addObstacle(std::array<std::string_view, N>& lines, size_t row, size_t col)
    {
        std::string newString {lines[row]};
        newString[col] = '#';
        return newString;
    }

    template <std::size_t N>
    bool isLoop(const std::array<std::string_view, N>& lines)
    {
        std::set<std::pair<size_t, size_t>> distinctPositions;
        int loopCount {0};

        std::pair<size_t, size_t> position {findStartPosition(lines)};
        std::pair<size_t, size_t> direction {getStartDirection(lines.at(position.second).at(position.first))};
        distinctPositions.insert(position);

        while (!isLeavingArea(position, lines))
        {
            std::pair<size_t, size_t> nextPosition {getNextPosition(position, direction)};

            if (isNextPositionObstacle(nextPosition, lines))
            {
                direction = turnRight(direction);
                continue;
            }
            else
                position = nextPosition;

            std::size_t oldDistinctCount {distinctPositions.size()};
            distinctPositions.insert(position);

            if (distinctPositions.size() == oldDistinctCount)
                ++loopCount;
            else
                loopCount = 0;
            
            if (loopCount == distinctPositions.size())
                return true;
        }

        return false;
    }

    template <std::size_t N>
    int parseAndCountDistinctLoops(const std::array<std::string_view, N>& lines)
    {
        std::set<std::pair<size_t, size_t>> originalPositions {getOriginalDistinctPositions<N>(lines)};
        
        int count {0};

        // this approach took a quarter of the time, but gave an inaccurate answer (1,617 vs the answer of 1,770)
        
        // for (std::pair<size_t, size_t> position : originalPositions)
        // {
        //     if (lines.at(position.second).at(position.first) == '.')
        //     {
        //         std::array<std::string_view, N> newLines {lines};
        //         newLines[position.second] = addObstacle(newLines, position.second, position.first);
        //         count += isLoop(newLines);
        //     }
        // }

        for (size_t i{0}; i < lines.size(); ++i)
        {
            for (size_t j{0}; j < lines.at(i).size(); ++j)
            {
                if (lines.at(i).at(j) == '.')
                {
                    std::array<std::string_view, N> newLines {lines};
                    newLines[i] = addObstacle(newLines, i, j);
                    count += isLoop(newLines);
                }
            }
        }

        return count;
    }
}

#endif // AOC6B_H
