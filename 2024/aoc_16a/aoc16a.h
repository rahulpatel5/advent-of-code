#ifndef AOC16A_H
#define AOC16A_H

#include <string_view>
#include <array>
#include "../../shared/Direction.h"
#include <vector>
#include <set>
#include <cassert>

#include <iostream>

/*
iterate each possible path, with +1000 score for turn and +1 for moving
collect lowest score
where a path exceeds the lowest score, or reaches a dead end, stop the path
return the lowest score
*/

namespace aoc16a
{
    using Move = std::pair<int, int>;
    using Moves = std::vector<Move>;

    using Position = std::pair<size_t, size_t>;
    using Positions = std::vector<Position>;
    using UniquePositions = std::set<Position>;

    using Scores = std::vector<int>;

    template <std::size_t N>
    Position getStartingPosition(const std::array<std::string_view, N> map)
    {
        for (size_t y{0}; y < map.size(); ++y)
        {
            for (size_t x{0}; x < map[y].size(); ++x)
            {
                if (map[y][x] == 'S')
                    return {x, y};
            }
        }
        throw std::invalid_argument("Could not find starting position.\n");
    }

    Position goInDirection(Position pos, Move move)
    {
        return {pos.first + move.first, pos.second + move.second};
    }

    template <std::size_t N>
    bool hitWall(Position pos, const std::array<std::string_view, N> map)
    {
        return map[pos.second][pos.first] == '#';
    }

    template <std::size_t N>
    bool reachExit(Position pos, const std::array<std::string_view, N> map)
    {
        return map[pos.second][pos.first] == 'E';
    }

    template <std::size_t N>
    bool goFreeSpace(Position pos, const std::array<std::string_view, N> map)
    {
        return map[pos.second][pos.first] == '.';
    }

    template <std::size_t N>
    Moves getOtherMoves(Position pos, Move move, const std::array<std::string_view, N>& map)
    {
        Moves next {};
        // check clockwise
        Move clockMove {Direction::turnClockwise(move)};
        Position clockPos {goInDirection(pos, clockMove)};
        if (goFreeSpace(clockPos, map) || reachExit(clockPos, map))
            next.push_back(clockMove);
        //check anti-clockwise
        Move antiClockMove {Direction::turnAntiClockwise(move)};
        Position antiClockPos {goInDirection(pos, antiClockMove)};
        if (goFreeSpace(antiClockPos, map) || reachExit(antiClockPos, map))
            next.push_back(antiClockMove);

        return next;
    }

    void deleteNonValidPaths(Positions& positions, Moves& movements, Scores& scores, int index)
    {
        positions.erase(std::next(positions.begin(), index), std::next(positions.begin(), index + 1));
        movements.erase(std::next(movements.begin(), index), std::next(movements.begin(), index + 1));
        scores.erase(std::next(scores.begin(), index), std::next(scores.begin(), index + 1));
    }

    bool isPositionAlreadyReached(Position pos, const UniquePositions& uniquePositions)
    {
        for (Position uniquePosition : uniquePositions)
        {
            if (uniquePosition == pos)
                return true;
        }
        return false;
    }

    int getLowestScore(const Scores& scores)
    {
        int lowest {0};
        for (int score : scores)
        {
            if (lowest == 0 || score < lowest)
                lowest = score;
        }
        return lowest;
    }

    template <std::size_t N>
    int parseAndGetScore(const std::array<std::string_view, N>& lines)
    {
        int score {0};
        constexpr Move move {Direction::east}; // set in puzzle
        Position start {getStartingPosition<N>(lines)};

        // set up starting position
        Positions positions {};
        positions.push_back(start);
        Moves movements {};
        movements.push_back(move);
        Scores scores {};
        scores.push_back(0);
        UniquePositions uniquePos {};
        uniquePos.insert(start);

        Scores finalScores {};
        bool noPathsLeft {false};
        while (!noPathsLeft)
        {
            std::vector<size_t> nonValidPaths {};
            UniquePositions treadOldGround {};
            assert((positions.size() == movements.size() && movements.size() == scores.size()) && "Info on the number of paths is inconsistent (position, move, score).\n");
            for (size_t i{0}; i < positions.size(); ++i)
            {
                Position currentPos {positions[i]};
                Move currentMove {movements[i]};
                int currentScore {scores[i]};
                // first traverse the current direction
                Position forward {goInDirection(positions[i], movements[i])};
                // check that position wasn't reached already
                // i.e. avoid loops, or paths that are longer than needed
                if (isPositionAlreadyReached(forward, uniquePos) && !reachExit(forward, lines))
                    nonValidPaths.push_back(i);
                // otherwise path is valid
                else if (goFreeSpace(forward, lines))
                {
                    positions[i] = forward;
                    ++scores[i];
                    treadOldGround.insert(forward);
                }
                // if new position is exit, then path is done
                else if (reachExit(forward, lines))
                {
                    finalScores.push_back(++scores[i]);
                    nonValidPaths.push_back(i);
                }
                // if hit wall, then current path is done
                else if (hitWall(forward, lines))
                    nonValidPaths.push_back(i);

                // next look at alternate paths, i.e. (anti-)clockwise
                Moves alternateMoves {getOtherMoves(currentPos, currentMove, lines)};
                for (Move otherMove : alternateMoves)
                {
                    Position nextSpace {goInDirection(currentPos, otherMove)};
                    // check that position was not reached before
                    if (isPositionAlreadyReached(nextSpace, uniquePos))
                        continue;
                    // if path valid, then add as a new path
                    else if (goFreeSpace(nextSpace, lines))
                    {
                        positions.push_back(nextSpace);
                        movements.push_back(otherMove);
                        // add 1,000 for turn and 1 for move
                        scores.push_back(currentScore + 1001);
                        treadOldGround.insert(nextSpace);
                    }
                    // if at exit, add 1,000 for turn and 1 for move
                    else if (reachExit(nextSpace, lines))
                        finalScores.push_back(currentScore + 1001);
                }
            }

            // this currently makes the code run longer without seeming to change the output
            // but this does seem to be a more reliable approach (adding new positions after the loop, to avoid dropping a path that may have ended up being shortest)
            for (Position pos : treadOldGround)
                uniquePos.insert(pos);
            // clean up by removing non-valid paths
            // go in reverse order to delete the right paths
            for (int j{static_cast<int>(nonValidPaths.size() - 1)}; j >= 0; --j)
                deleteNonValidPaths(positions, movements, scores, nonValidPaths[j]);
            
            if (positions.size() == 0)
                noPathsLeft = true;
        }

        return getLowestScore(finalScores);
    }
}

#endif // AOC16A_H
