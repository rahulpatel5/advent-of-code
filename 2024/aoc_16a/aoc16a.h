#ifndef AOC16A_H
#define AOC16A_H

#include <string_view>
#include <array>
#include "../../shared/Direction.h"
#include <vector>
#include <set>
#include <cassert>
#include <algorithm>
#include <map>

#include <iostream>

/*
iterate each possible path, with +1000 score for turn and +1 for moving
collect lowest score
where a path exceeds the lowest score, or reaches a dead end, stop the path
return the lowest score

traversing the maze seems to be failing
another option is to revise the map
find all dead-ends and fill them in, back up to a cross-roads
i.e. find all free spaces surrounded by three hashes and make those a hash
this reduces wasted time traversing dead ends
it might make it more reasonable to traverse all paths?
the current issue may be that there is a winding path getting dropped
when it would end up being the shortest

can try memoisation, i.e. collect unique positions (x, y)
if reach a point and have a higher (or equal) score, then stop the path
otherwise continue
need to collect position, score and direction
might still lead to some paths continuing when they're longer?
*/

namespace aoc16a
{
    using Move = std::pair<int, int>;
    using Moves = std::vector<Move>;

    using Position = std::pair<size_t, size_t>;
    using Positions = std::vector<Position>;
    // this collects the score at each position
    using MapDict = std::map<Position, int>;

    using Scores = std::vector<int>;

    template <std::size_t N>
    bool isDeadEnd(const std::array<std::string, N>& map, size_t x, size_t y)
    {
        int count {0};
        for (Move move : Direction::allDirections)
        {
            if (map.at(y + move.second).at(x + move.first) == '#')
                ++count;
        }
        return count >= 3;
    }

    template <std::size_t N>
    void reviseMap(std::array<std::string, N>& map)
    {
        int count {1};
        while (count > 0)
        {
            count = 0;
            for (size_t y{1}; y < map.size() - 1; ++y)
            {
                for (size_t x{1}; x < map.at(y).size() - 1; ++x)
                {
                    if (map[y][x] != '.')
                        continue;
                    if (isDeadEnd(map, x, y))
                    {
                        ++count;
                        map[y][x] = '#';
                    }
                }
            }
        }
    }

    template <std::size_t N>
    Position getStartingPosition(const std::array<std::string_view, N>& map)
    {
        for (size_t y{0}; y < map.size(); ++y)
        {
            if (map[y].find('S') == std::string_view::npos)
                continue;
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
    bool hitWall(Position pos, const std::array<std::string, N>& map)
    {
        return map[pos.second][pos.first] == '#';
    }

    template <std::size_t N>
    bool reachExit(Position pos, const std::array<std::string, N>& map)
    {
        return map[pos.second][pos.first] == 'E';
    }

    template <std::size_t N>
    bool isFree(Position pos, const std::array<std::string, N>& map)
    {
        return map[pos.second][pos.first] == '.';
    }

    template <std::size_t N>
    Moves getOtherMoves(Position pos, Move move, const std::array<std::string, N>& map)
    {
        Moves next {};
        // check clockwise
        Move clockMove {Direction::turnClockwise(move)};
        Position clockPos {goInDirection(pos, clockMove)};
        if (isFree(clockPos, map) || reachExit(clockPos, map))
            next.push_back(clockMove);
        //check anti-clockwise
        Move antiClockMove {Direction::turnAntiClockwise(move)};
        Position antiClockPos {goInDirection(pos, antiClockMove)};
        if (isFree(antiClockPos, map) || reachExit(antiClockPos, map))
            next.push_back(antiClockMove);

        return next;
    }

    void deleteNonValidPaths(Positions& positions, Moves& movements, Scores& scores, int index)
    {
        positions.erase(std::next(positions.begin(), index), std::next(positions.begin(), index + 1));
        movements.erase(std::next(movements.begin(), index), std::next(movements.begin(), index + 1));
        scores.erase(std::next(scores.begin(), index), std::next(scores.begin(), index + 1));
    }

    bool isValidPosition(Position pos, int score, const MapDict& dict)
    {
        return dict.at(pos) >= score;
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
        MapDict mapPos {};
        mapPos[start] = 0;

        Scores finalScores {};
        // can improve efficiency by tracking the lowest final score
        // and dropping paths that are higher than this
        int lowestFinalScore {};
        bool noPathsLeft {false};

        // remove dead ends from map, to make finding valid paths faster
        std::array<std::string, N> newMap {};
        for (size_t i{0}; i < lines.size(); ++i)
            newMap[i] = std::string(lines.at(i));
        reviseMap<N>(newMap);

        // reached after lots of testing of the puzzle
        while (!noPathsLeft)
        {
            std::vector<size_t> nonValidPaths {};
            assert((positions.size() == movements.size() && movements.size() == scores.size()) && "Info on the number of paths is inconsistent (position, move, score).\n");
            Positions positionsToAdd {};
            Moves movesToAdd {};
            Scores scoresToAdd {};

            int way {0};
            for (size_t i{0}; i < positions.size(); ++i)
            {
                Position currentPos {positions[i]};
                Move currentMove {movements[i]};
                int currentScore {scores[i]};
                // first traverse the current direction
                Position forward {goInDirection(positions[i], movements[i])};
                // if path has higher score than current lowest
                // then there's no point continuing. End early for more speed
                // also short circuit long, pointless paths with max score
                if (lowestFinalScore > 0 && scores[i] >= lowestFinalScore)
                {
                    nonValidPaths.push_back(i);
                    continue;
                }
                // check position wasn't reached already with a lower score
                // i.e. avoid loops, or paths that are longer than needed
                if (mapPos.find(forward) != mapPos.end() && !isValidPosition(forward, scores[i] + 1, mapPos))
                    nonValidPaths.push_back(i);
                // otherwise path is valid
                else if (isFree(forward, newMap))
                {
                    positions[i] = forward;
                    ++scores[i];
                    mapPos[forward] = scores[i];
                }
                // if new position is exit, then path is done
                else if (reachExit(forward, newMap))
                {
                    finalScores.push_back(scores[i] + 1);
                    if (lowestFinalScore == 0 || scores[i] + 1 < lowestFinalScore)
                        lowestFinalScore = scores[i] + 1;
                    nonValidPaths.push_back(i);
                    mapPos[forward] = scores[i] + 1;
                }
                // if hit wall, then current path is done
                else if (hitWall(forward, newMap))
                    nonValidPaths.push_back(i);

                // next look at alternate paths, i.e. (anti-)clockwise
                Moves alternateMoves {getOtherMoves(currentPos, currentMove, newMap)};
                for (Move otherMove : alternateMoves)
                {
                    Position nextSpace {goInDirection(currentPos, otherMove)};
                    // check position was not reached before with lower score
                    if (mapPos.find(nextSpace) != mapPos.end() && !isValidPosition(nextSpace, currentScore + 1001, mapPos))
                        continue;
                    // if path valid, then add as a new path
                    else if (isFree(nextSpace, newMap))
                    {
                        // need to add these to temporary objects and add later
                        // as otherwise we could add extra loops in the for loop
                        positionsToAdd.push_back(nextSpace);
                        movesToAdd.push_back(otherMove);
                        // add 1,000 for turn and 1 for move
                        scoresToAdd.push_back(currentScore + 1001);
                        mapPos[nextSpace] = currentScore + 1001;
                    }
                    // if at exit, add 1,000 for turn and 1 for move
                    else if (reachExit(nextSpace, newMap))
                    {
                        finalScores.push_back(currentScore + 1001);
                        if (lowestFinalScore == 0 || currentScore + 1001 < lowestFinalScore)
                            lowestFinalScore = currentScore + 1001;
                        mapPos[nextSpace] = currentScore + 1001;
                    }
                }
            }

            for (size_t j{0}; j < positionsToAdd.size(); ++j)
            {
                positions.push_back(positionsToAdd[j]);
                movements.push_back(movesToAdd[j]);
                scores.push_back(scoresToAdd[j]);
            }
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
