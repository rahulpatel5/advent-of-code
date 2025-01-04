#ifndef AOC20B_H
#define AOC20B_H

#include <string_view>
#include <array>
#include <vector>
#include "../../shared/Direction.h"
#include <set>
#include <map>
#include <stdexcept>

/*
re-use solution to part 1
change logic of cheating to allow traversing more than 2 space walls
assuming that cheat stops as soon as reach a free space (or exit)
keep track of cheat spaces travelled and add to count extra steps

the big complexity is how to traverse wall tiles?
e.g. need to make sure not to go in circles
but at the same time want to change direction, possibly twice
it will also likely greatly increase the number of paths to traverse
so will take longer than the c3 min for part 1

create a map from initial (no cheat) run  with distance to end from each pos
then find way to cheat over 20 spaces and find next point
take steps from start + cheat steps + distance from new start (from map)
this should allow getting target saving time with minimal effort

I'm not getting why I'm getting under the test puzzle result
it suggests I'm missing something in the instructions, but I can't see what
I did assume that any time there's a return to the track, the cheat ends
that's not necessarily the case?
*/

namespace aoc20b
{
    using Position = std::pair<size_t, size_t>;
    using Positions = std::vector<Position>;
    using Path = std::vector<Position>;
    using Paths = std::vector<Path>;
    using UniquePositions = std::set<Position>;
    using UniqueStartEnd = std::set<std::pair<Position, Position>>;
    using Move = std::pair<int, int>;
    using Moves = std::vector<Move>;

    using Distances = std::map<Position, int>;

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

    Distances getNonCheatDistances(const Path& nonCheatPath)
    {
        Distances distances {};
        int steps {0};
        // go backwards over non cheat path
        // starting at 0 ps needed at exit
        for (auto it {nonCheatPath.rbegin()}; it != nonCheatPath.rend(); ++it)
            distances[*it] = steps++;
        return distances;
    }

    template <std::size_t N>
    bool isMoveUnsafe(Position pos, Move move, const std::array<std::string_view, N>& lines)
    {
        return ((pos.second == 0 && move == Direction::north) || (pos.first == lines[0].size() - 1 && move == Direction::east) || (pos.second == lines.size() - 1 && move == Direction::south) || (pos.first == 0 && move == Direction::west));
    }

    template <std::size_t N, int Ttarget, int TmaxCheat>
    int getValidCheats(Position startPoint, const std::array<std::string_view, N>& lines, Position end, const Distances& nonCheatDistances, int steps, int shortestPath)
    {
        // we can short circuit and avoid running unnecessary start points
        // if we use the target save time and distance to the exit
        // i.e. even if the cheat saved time, it wouldn't be under target
        // we subtract 2 since the minimum cheat is 2ps
        if (shortestPath - steps - 2 < Ttarget)
            return 0;
        
        int count {0};
        int time {1};
        UniquePositions currentPositions {};
        currentPositions.insert(startPoint);
        UniquePositions uniquePositions {};
        while (time <= TmaxCheat)
        {
            UniquePositions nextPositions {};
            for (Position pos : currentPositions)
            {
                for (Move move : Direction::allDirections)
                {
                    // if would move out of bounds, skip
                    if (isMoveUnsafe<N>(pos, move, lines))
                        continue;
                    
                    Position step {getNewPosition(pos, move)};
                    // if position is in uniquePositions already, skip
                    if (uniquePositions.find(step) != uniquePositions.end())
                        continue;
                    // 
                    nextPositions.insert(step);
                    if (isFreeSpace(step, lines) || isExit(step, end))
                    {
                        uniquePositions.insert(step);
                        // count if it meets saving target
                        if (shortestPath >= steps + time + nonCheatDistances.at(step) + Ttarget)
                            ++count;
                    }
                }
            }
            currentPositions = nextPositions;
            ++time;
        }
        return count;
    }

    template <std::size_t N, int Ttarget, int TmaxCheat>
    int countCheatsSavingTargetTime(const std::array<std::string_view, N>& lines, const Path& shortestNonCheatPath, const Distances& nonCheatDistances, Position start, Position end)
    {
        int count {0};
        // we'll be counting time to reach exit from different positions
        // so need to track distance from start position
        int steps {0};
        for (Position oldPathPos : shortestNonCheatPath)
        {
            count += getValidCheats<N, Ttarget, TmaxCheat>(oldPathPos, lines, end, nonCheatDistances, steps, static_cast<int>(shortestNonCheatPath.size() - 1));
            ++steps;
        }
        return count;
    }

    template <std::size_t N, int Ttarget, int TmaxCheat>
    int parseAndCountSavingCheats(const std::array<std::string_view, N>& lines)
    {
        Position start {getPosition<N>(lines, 'S')};
        Position end {getPosition<N>(lines, 'E')};
        Path shortestNonCheatPath {getShortestNonCheatPath<N>(lines, start, end)};
        Distances distancesNonCheatPath {getNonCheatDistances(shortestNonCheatPath)};
        return countCheatsSavingTargetTime<N, Ttarget, TmaxCheat>(lines, shortestNonCheatPath, distancesNonCheatPath, start, end);
    }
}

#endif // AOC20B_H
