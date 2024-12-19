#ifndef AOC18B_H
#define AOC18B_H

#include <string_view>
#include <array>
#include "../../shared/Direction.h"
#include <vector>
#include <string>
#include <set>

/*
re-use solution to part 1 to find where exit can be reached
set up new map with extra blocked spaces and test if exit can be reached
to be efficient, use approach of adding half of remaining blocked spaces
if can reach exit, continue looking in second half
if can't reach exit, look in first half
continue splitting in half to cut down
return earliest blocker
*/

namespace aoc18b
{
    using Position = std::pair<size_t, size_t>;
    using Positions = std::vector<Position>;
    using UniquePositions = std::set<Position>;
    using Move = std::pair<int, int>;
    using Moves = std::array<Move, 4>;
    using Map = std::vector<std::string>;
    using Limits = std::pair<int, int>;

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

    // overload above function for non-constexpr int (Tbytes)
    // keeping Tbytes name to be consistent with function above
    template <std::size_t N>
    Positions getBlockedSpaces(const std::array<std::string_view, N>& lines, int Tbytes)
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

    // bit wasteful to duplicate above function, but simpler in this case
    template <std::size_t N>
    Positions getAllBlockedSpaces(const std::array<std::string_view, N>& lines)
    {
        Positions positions {};
        for (std::string_view line : lines)
        {
            size_t comma {line.find(',')};
            size_t digit1 {std::stoul(std::string(line.substr(0, comma)))};
            size_t digit2 {std::stoul(std::string(line.substr(comma + 1)))};
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
    bool canReachExit(const Map& map, Position start, Position exit)
    {
        Positions positions {};
        positions.push_back(start);
        Moves compass {Direction::allDirections};
        UniquePositions uniquePos {};
        uniquePos.insert(start);
        size_t uniquePosSize {0};
        while (!hasReachedExit(positions, exit) && uniquePosSize != uniquePos.size())
        {
            uniquePosSize = uniquePos.size();
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
        return hasReachedExit(positions, exit);
    }

    template <std::size_t N, std::size_t Tside, int Tbytes>
    Position findEarliestBlocker(const Map& map, Position start, Position exit, const std::array<std::string_view, N>& lines)
    {
        Positions allBlockedSpaces {getAllBlockedSpaces<N>(lines)};
        // these are sizes, not indexes
            // first = lower limit = can reach exit
            // second = upper limit = can't reach exit
        Limits limits {Tbytes, static_cast<int>(allBlockedSpaces.size())};
        while (limits.second - limits.first > 1)
        {
            // get half-way point (rounded down)
            int newBlockedNumber {(limits.second + limits.first) / 2};
            Positions newBlockedSpaces {getBlockedSpaces<N>(lines, newBlockedNumber)};
            Map newMap {setUpFilledMap<Tside>(newBlockedSpaces)};
            bool doesReachExit {canReachExit<Tside>(newMap, start, exit)};
            if (doesReachExit)
                limits.first = newBlockedNumber;
            else
                limits.second = newBlockedNumber;
        }
        int earliestBlockerNumber {limits.second};
        Positions finalBlockedSpaces {getBlockedSpaces<N>(lines, earliestBlockerNumber)};
        return finalBlockedSpaces.back();
    }

    template <std::size_t N, std::size_t Tside, int Tbytes>
    Position parseAndGetFirstExitBlocker(const std::array<std::string_view, N>& lines)
    {
        constexpr Position start {0, 0};
        constexpr Position exit {Tside - 1, Tside - 1};
        Positions blockedSpaces {getBlockedSpaces<N, Tbytes>(lines)};
        Map filledMap {setUpFilledMap<Tside>(blockedSpaces)};
        return findEarliestBlocker<N, Tside, Tbytes>(filledMap, start, exit, lines);
    }
}

#endif // AOC18B_H
