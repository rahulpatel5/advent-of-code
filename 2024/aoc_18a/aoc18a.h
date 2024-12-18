#ifndef AOC18A_H
#define AOC18A_H

#include <string_view>
#include <array>
#include "../../shared/Direction.h"
#include <vector>
#include <string>
#include <set>

/*
read in specified first n coordinates for corrupted spaces
construct map and add corrupted spaces
(unclear if puzzle involves spaces being added over time?)
iterate different paths and find the shortest
return shortest steps

first try direct route and assuming earliest will be the minimum
*/

namespace aoc18a
{
    using Position = std::pair<size_t, size_t>;
    using Positions = std::vector<Position>;
    using UniquePositions = std::set<Position>;
    using Move = std::pair<int, int>;
    using Moves = std::array<Move, 4>;
    using Map = std::vector<std::string>;

    template <std::size_t N, int Tbytes>
    Positions getBlockedSpaces(const std::array<std::string_view, N>& lines)
    {
        Positions positions {};
        for (size_t i{0}; i < Tbytes; ++i)
        {
            size_t comma {lines[i].find(',')};
            size_t digit1 {std::stoul(std::string(lines[i].substr(0, comma)))};
            size_t digit2 {std::stoul(std::string(lines[i].substr(comma + 1)))};
            positions.push_back({digit1, digit2});
        }
        return positions;
    }

    template <std::size_t Tside>
    Map setUpFilledMap(const Positions& spaces)
    {
        Map map {};
        for (size_t y{0}; y < Tside; ++y)
        {
            std::string row {};
            for (size_t x{0}; x < Tside; ++x)
                row.append(".");
            map.push_back(row);
        }
        for (const Position& space : spaces)
            map[space.second][space.first] = '#';
        return map;
    }

    template <std::size_t Tside>
    bool isGoingOutOfBounds(Position pos, Move direction)
    {
        return ((pos.second == 0 && direction == Direction::north) || (pos.first == Tside - 1 && direction == Direction::east) || (pos.second == Tside - 1 && direction == Direction::south) || (pos.first == 0 && direction == Direction::west));
    }

    bool hasReachedExit(const Positions& positions, Position exit)
    {
        for (const Position& position : positions)
        {
            if (position.first == exit.first && position.second == exit.second)
                return true;
        }
        return false;
    }

    bool isBlockedSpace(Position position, const Map& map)
    {
        return map[position.second][position.first] == '#';
    }

    bool isAlreadyReachedPosition(Position position, const UniquePositions& uniquePositions)
    {
        return uniquePositions.count(position) > 0;
    }

    template <std::size_t Tside>
    int getMinMovesToExit(const Map& map, Position start, Position exit)
    {
        Positions positions {};
        positions.push_back(start);
        Moves compass {Direction::allDirections};
        UniquePositions uniquePos {};
        uniquePos.insert(start);
        int steps {0};
        while (!hasReachedExit(positions, exit))
        {
            ++steps;
            Positions newPositions {};
            for (const Position& position : positions)
            {
                for (const Move direction : compass)
                {
                    Position nextPos {position.first + direction.first, position.second + direction.second};
                    if (!isGoingOutOfBounds<Tside>(position, direction) && !isBlockedSpace(nextPos, map) && !isAlreadyReachedPosition(nextPos, uniquePos))
                    {
                        newPositions.push_back(nextPos);
                        uniquePos.insert(nextPos);
                    }
                }
            }
            positions = newPositions;
        }
        return steps;
    }

    template <std::size_t N, std::size_t Tside, int Tbytes>
    int parseAndGetMinSteps(const std::array<std::string_view, N>& lines)
    {
        constexpr Position start {0, 0};
        constexpr Position exit {Tside - 1, Tside - 1};
        Positions blockedSpaces {getBlockedSpaces<N, Tbytes>(lines)};
        Map filledMap {setUpFilledMap<Tside>(blockedSpaces)};
        return getMinMovesToExit<Tside>(filledMap, start, exit);
    }
}

#endif // AOC18A_H
