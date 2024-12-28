#ifndef AOC16B_H
#define AOC16B_H

#include <string_view>
#include <array>
#include "../../shared/Direction.h"
#include <vector>
#include <set>
#include <cassert>
#include <algorithm>
#include <map>

/*
re-use solution to part 1
now collect the paths of those that reach the end
with all the final paths, use set to identify unique positions
i.e. that occur on at least one of the best paths

a best path may be dropped because of one path having one less turn
so need to account for that
*/

namespace aoc16b
{
    using Move = std::pair<int, int>;
    using Moves = std::vector<Move>;

    using Position = std::pair<size_t, size_t>;
    using Positions = std::vector<Position>;
    using Paths = std::vector<Positions>;
    using UniquePositions = std::set<Position>;
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

    void deleteNonValidPaths(Positions& positions, Moves& movements, Scores& scores, Paths& paths, int index)
    {
        positions.erase(std::next(positions.begin(), index), std::next(positions.begin(), index + 1));
        movements.erase(std::next(movements.begin(), index), std::next(movements.begin(), index + 1));
        scores.erase(std::next(scores.begin(), index), std::next(scores.begin(), index + 1));
        paths.erase(std::next(paths.begin(), index), std::next(paths.begin(), index + 1));
    }

    bool isValidPosition(Position pos, int score, const MapDict& dict)
    {
        // need to account that a given position may be 1000 lower
        // due to one path having one fewer turn
        if (dict.at(pos) == score - 1000)
            return true;
        else
            return dict.at(pos) >= score;
    }

    Paths getBestPaths(const Paths& paths, const Scores& scores)
    {
        assert((paths.size() == scores.size()) && "Final number of paths and scores is inconsistent.\n");
        int lowestScore {0};
        for (int score : scores)
        {
            if (lowestScore == 0)
                lowestScore = score;
            else if (score < lowestScore)
                lowestScore = score;
        }
        Paths best {};
        for (size_t i{0}; i < paths.size(); ++i)
        {
            if (scores.at(i) == lowestScore)
                best.push_back(paths.at(i));
        }
        return best;
    }

    size_t getTilesOnBestPaths(const Paths& paths)
    {
        UniquePositions unique {};
        for (const Positions& positions : paths)
        {
            for (const Position& pos : positions)
            {
                unique.insert(pos);
            }
        }
        return unique.size();
    }

    template <std::size_t N>
    size_t parseAndGetBestTiles(const std::array<std::string_view, N>& lines)
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

        // collect paths through maze
        Paths paths {};
        paths.push_back(positions);
        Paths finalPaths {};

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
            assert((positions.size() == movements.size() && movements.size() == scores.size() && scores.size() == paths.size()) && "Info on the number of paths is inconsistent (position, move, score, paths).\n");
            Positions positionsToAdd {};
            Moves movesToAdd {};
            Scores scoresToAdd {};
            Paths pathsToAdd {};

            int way {0};
            for (size_t i{0}; i < positions.size(); ++i)
            {
                Position currentPos {positions[i]};
                Move currentMove {movements[i]};
                int currentScore {scores[i]};
                Positions currentPath {paths[i]};
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
                    paths[i].push_back(forward);
                }
                // if new position is exit, then path is done
                else if (reachExit(forward, newMap))
                {
                    finalScores.push_back(scores[i] + 1);
                    if (lowestFinalScore == 0 || scores[i] + 1 < lowestFinalScore)
                        lowestFinalScore = scores[i] + 1;
                    nonValidPaths.push_back(i);
                    mapPos[forward] = scores[i] + 1;
                    paths[i].push_back(forward);
                    finalPaths.push_back(paths[i]);
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
                        Positions tempPath {currentPath};
                        tempPath.push_back(nextSpace);
                        pathsToAdd.push_back(tempPath);
                    }
                    // if at exit, add 1,000 for turn and 1 for move
                    else if (reachExit(nextSpace, newMap))
                    {
                        finalScores.push_back(currentScore + 1001);
                        if (lowestFinalScore == 0 || currentScore + 1001 < lowestFinalScore)
                            lowestFinalScore = currentScore + 1001;
                        mapPos[nextSpace] = currentScore + 1001;
                        Positions tempPath {currentPath};
                        tempPath.push_back(nextSpace);
                        finalPaths.push_back(tempPath);
                    }
                }
            }

            for (size_t j{0}; j < positionsToAdd.size(); ++j)
            {
                positions.push_back(positionsToAdd[j]);
                movements.push_back(movesToAdd[j]);
                scores.push_back(scoresToAdd[j]);
                paths.push_back(pathsToAdd[j]);
            }
            // clean up by removing non-valid paths
            // go in reverse order to delete the right paths
            for (int j{static_cast<int>(nonValidPaths.size() - 1)}; j >= 0; --j)
                deleteNonValidPaths(positions, movements, scores, paths, nonValidPaths[j]);
            if (positions.size() == 0)
                noPathsLeft = true;
        }

        Paths bestPaths {getBestPaths(finalPaths, finalScores)};
        return getTilesOnBestPaths(bestPaths);
    }
}

#endif // AOC16B_H
