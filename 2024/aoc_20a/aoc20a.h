#ifndef AOC20A_H
#define AOC20A_H

#include <string_view>
#include <array>
#include <vector>
#include "../../shared/Direction.h"
#include <set>
#include <stdexcept>

/*
find the shortest non-cheating way through maze
(could keep that path and use for the rest. But no guarantee there isn't a shorter alternative path after cheating)
add cheating rule and run through maze again
save unique starting and exiting points
count how many cheat paths save at least the target time
*/

namespace aoc20a
{
    using Position = std::pair<size_t, size_t>;
    using Positions = std::vector<Position>;
    using Path = std::vector<Position>;
    using Paths = std::vector<Path>;
    using UniquePositions = std::set<Position>;
    using UniqueStartEnd = std::set<std::pair<Position, Position>>;
    using Move = std::pair<int, int>;
    using Moves = std::vector<Move>;

    template <std::size_t N>
    Position getPosition(const std::array<std::string_view, N>& lines, char c)
    {
        for (size_t y{0}; y < lines.size(); ++y)
        {
            if (lines[y].find(c) == std::string_view::npos)
                continue;
            for (size_t x{0}; x < lines[y].size(); ++x)
            {
                if (lines[y][x] == c)
                    return {x, y};
            }
        }
        throw std::invalid_argument("The position was not found on the map.\n");
    }

    Position getNewPosition(Position pos, Move move)
    {
        return {pos.first + move.first, pos.second + move.second};
    }

    template <std::size_t N>
    bool isFreeSpace(Position pos, const std::array<std::string_view, N>& lines)
    {
        return lines[pos.second][pos.first] == '.';
    }

    bool isExit(Position pos, Position exit)
    {
        return pos == exit;
    }

    template <std::size_t N>
    bool isWall(Position pos, const std::array<std::string_view, N>& lines)
    {
        return lines[pos.second][pos.first] == '#';
    }

    template <std::size_t N>
    Moves getValidMoves(Position pos, const std::array<std::string_view, N>& lines, Position end)
    {
        Moves moves {};
        for (Move move : Direction::allDirections)
        {
            if (isFreeSpace(getNewPosition(pos, move), lines) || isExit(getNewPosition(pos, move), end))
                moves.push_back(move);
        }
        return moves;
    }

    template <std::size_t N>
    Path getShortestNonCheatPath(const std::array<std::string_view, N>& lines, Position start, Position end)
    {
        Paths paths {};
        paths.push_back({start});
        UniquePositions unique {};
        unique.insert(start);
        bool reachedExit {false};
        size_t shortestPath {};
        while (!reachedExit)
        {
            std::vector<size_t> indicesToAdd {};
            Positions nextPosToAdd {};
            Paths origPathToAdd {};
            for (size_t i{0}; i < paths.size(); ++i)
            {
                Position lastPos {paths[i].back()};
                Moves validMoves {getValidMoves(lastPos, lines, end)};
                bool isFirstValid {true};
                for (Move move : validMoves)
                {
                    Position nextPos {getNewPosition(lastPos, move)};
                    if (unique.find(nextPos) != unique.end())
                        continue;
                    if (isFirstValid)
                    {
                        paths[i].push_back(nextPos);
                        unique.insert(nextPos);
                        isFirstValid = false;
                        if (isExit(nextPos, end))
                        {
                            reachedExit = true;
                            shortestPath = i;
                        }
                    }
                    else
                    {
                        indicesToAdd.push_back(i);
                        nextPosToAdd.push_back(nextPos);
                        origPathToAdd.push_back(paths[i]);
                    }
                }
            }
            for (size_t j{0}; j < indicesToAdd.size(); ++j)
            {
                if (unique.find(nextPosToAdd[j]) != unique.end())
                    continue;
                Path newPath {origPathToAdd[j]};
                newPath.back() = nextPosToAdd[j];
                paths.push_back(newPath);
                unique.insert(nextPosToAdd[j]);
                if (isExit(nextPosToAdd[j], end))
                {
                    reachedExit = true;
                    shortestPath = paths.size() + j;
                }
            }
        }
        return paths[shortestPath];
    }

    template <std::size_t N>
    bool isDoubleMoveUnsafe(Position pos, Move move, const std::array<std::string_view, N>& lines)
    {
        return ((pos.second < 2 && move == Direction::north) || (pos.first > lines[0].size() - 3 && move == Direction::east) || (pos.second > lines.size() - 3 && move == Direction::south) || (pos.first < 2 && move == Direction::west));
    }

    template <std::size_t N>
    Positions getValidCheatStarts(Position pos, const std::array<std::string_view, N>& lines, Position end, const UniqueStartEnd& uniqueStartEnd)
    {
        Positions positions {};
        for (Move move : Direction::allDirections)
        {
            if (isDoubleMoveUnsafe(pos, move, lines))
                continue;
            // we're double moving for the cheat rule
            // this may be the place to work on, if the solution isn't right
            Position step1 {getNewPosition(pos, move)};
            if (!isWall(step1, lines))
                continue;
            Position step2 {getNewPosition(step1, move)};
            std::pair<Position, Position> oldNew {pos, step2};
            if (uniqueStartEnd.count(oldNew) == 0 && (isFreeSpace(step2, lines) || isExit(step2, end)))
                positions.push_back(step2);
        }
        return positions;
    }

    template <std::size_t N, int Ttarget>
    int countCheatsSavingTargetTime(const std::array<std::string_view, N>& lines, const Path& shortestNonCheatPath, Position start, Position end)
    {
        int count {0};
        UniqueStartEnd uniqueStartEnd {};
        // we'll be counting time to reach exit from different positions
        // so need to track distance from start position
        int steps {0};
        for (Position oldPathPos : shortestNonCheatPath)
        {
            Positions cheatStarts {getValidCheatStarts(oldPathPos,lines, end, uniqueStartEnd)};
            for (Position newStart : cheatStarts)
            {
                if (isExit(newStart, end))
                {
                    if ((shortestNonCheatPath.size() - 1) >= (steps + 2) + Ttarget)
                        ++count;
                    continue;
                }
                uniqueStartEnd.insert({oldPathPos, newStart});
                Path newPath {getShortestNonCheatPath<N>(lines, newStart, end)};
                // account for steps moved from start and 2 cheat steps
                // subtract 1 as since both 'paths' include start, which
                // would get double counted otherwise (e.g. in steps)
                if ((shortestNonCheatPath.size() - 1) >= ((newPath.size() - 1) + steps + 2) + Ttarget)
                    ++count;
            }
            ++steps;
        }
        return count;
    }

    template <std::size_t N, int Ttarget>
    int parseAndCountSavingCheats(const std::array<std::string_view, N>& lines)
    {
        Position start {getPosition<N>(lines, 'S')};
        Position end {getPosition<N>(lines, 'E')};
        Path shortestNonCheatPath {getShortestNonCheatPath<N>(lines, start, end)};
        return countCheatsSavingTargetTime<N, Ttarget>(lines, shortestNonCheatPath, start, end);
    }
}

#endif // AOC20A_H
