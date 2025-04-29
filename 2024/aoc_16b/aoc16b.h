#ifndef AOC16B_H
#define AOC16B_H

#include "Maze.h"
#include <string_view>
#include <array>
#include "../../shared/Direction.h"
#include <vector>
#include <set>
#include <cassert>
#include <algorithm>
#include <map>
#include <tuple>

#include <iostream>

/*
re-use solution to part 1
now collect the paths of those that reach the end
with all the final paths, use set to identify unique positions
i.e. that occur on at least one of the best paths

a best path may be dropped because one path has one less turn
so need to account for that
*/

namespace aoc16b
{
    constexpr int turn {1'000}; // score for turning
    constexpr int space {1}; // score for traversing one space

    using Move = std::pair<int, int>;
    using Moves = std::vector<Move>;

    using Position = std::pair<size_t, size_t>;
    using Positions = std::vector<Position>;
    using Paths = std::vector<Positions>;
    using UniquePositions = std::set<Position>;

    using Score = int;
    using Scores = std::vector<Score>;
    // distances from points after intersection to next intersection
    using InterKey = std::pair<Position, Move>;
    using InterVal = std::tuple<Position, Move, Score>;
    using IntersectionMap = std::map<InterKey, InterVal>;
    using ShortestDistances = std::map<Position, Score>;

    using Indexes = std::vector<size_t>;

    Position goInDirection(Position pos, Move move)
    {
        return {pos.first + move.first, pos.second + move.second};
    }

    template <std::size_t N>
    bool hitWall(Position pos, const Maze<N>& maze)
    {
        return maze.getPos(pos.first, pos.second) == '#';
    }

    template <std::size_t N>
    bool reachExit(Position pos, const Maze<N>& maze)
    {
        return pos == maze.getEnd();
    }

    template <std::size_t N>
    bool isFree(Position pos, const Maze<N>& maze)
    {
        return maze.getPos(pos.first, pos.second) == '.';
    }

    template <std::size_t N>
    Moves getOtherMoves(Position pos, Move move, const Maze<N>& maze)
    {
        Moves next {};
        // check clockwise
        Move clockMove {Direction::turnClockwise(move)};
        Position clockPos {goInDirection(pos, clockMove)};
        if (isFree(clockPos, maze) || reachExit(clockPos, maze) || clockPos == maze.getStart())
            next.push_back(clockMove);
        //check anti-clockwise
        Move antiClockMove {Direction::turnAntiClockwise(move)};
        Position antiClockPos {goInDirection(pos, antiClockMove)};
        if (isFree(antiClockPos, maze) || reachExit(antiClockPos, maze) || antiClockPos == maze.getStart())
            next.push_back(antiClockMove);

        return next;
    }

    template <std::size_t N>
    Moves getInitialMoves(Position pos, const Maze<N>& maze)
    {
        Moves next {};
        for (Move move : Direction::allDirections)
        {
            Position newPos {goInDirection(pos, move)};
            if (isFree(newPos, maze) || pos == maze.getStart())
            {
                next.push_back(move);
            }
        }
        return next;
    }

    template <std::size_t N>
    void fillPath(const Positions& path, Maze<N>& maze)
    {
        // the first and last positions don't count
        for (size_t i{1}; i < path.size() - 1; ++i)
        {
            maze.fillMaze(path.at(i).first, path.at(i).second);
        }
    }

    template <std::size_t N>
    void fillIfPointlessPath(size_t x, size_t y, Maze<N>& maze)
    {
        Paths paths {};
        Positions current {};
        Scores scores {};
        Moves nextMoves {getInitialMoves({x, y}, maze)};

        std::vector<char> pathAtEnd {};
        for (Move mv : nextMoves)
        {
            // whether path at end - '0' for false, '1' for true
            pathAtEnd.push_back('0');
            Position po {x, y};
            current.push_back(po);
            paths.push_back({po});
            if (!isFree(goInDirection(po, {-mv.first, -mv.second}), maze) && (mv == Direction::east || mv == Direction::north))
                scores.push_back(turn);
            else
                scores.push_back(0);
        }

        int ended {0};
        while (ended < pathAtEnd.size())
        {
            for (size_t i{0}; i < current.size(); ++i)
            {
                if (pathAtEnd.at(i) == '0')
                {
                    Position newPos {goInDirection(current.at(i), nextMoves.at(i))};

                    int walls {maze.countWalls(newPos.first, newPos.second)};
                    current[i] = newPos;
                    scores[i] += space;
                    Position nextNextPos {goInDirection(newPos, nextMoves.at(i))};
                    if (std::find(paths.at(i).begin(), paths.at(i).end(), newPos) != paths.at(i).end() || walls < 2 || reachExit(newPos, maze) || newPos == maze.getStart())
                    {
                        pathAtEnd[i] = '1';
                        ++ended;
                        paths[i].push_back(newPos);
                        if (!isFree(nextNextPos, maze))
                            scores[i] += turn;
                    }
                    else
                    {
                        paths[i].push_back(newPos);
                        if (!isFree(nextNextPos, maze) && !reachExit(nextNextPos, maze) && !(nextNextPos == maze.getStart()))
                        {
                            Moves newMove {getOtherMoves(newPos, nextMoves.at(i), maze)};
                            assert(newMove.size() == 1 && "There isn't only one new move, as was expected.\n");
                            nextMoves[i] = newMove.at(0);
                            scores[i] += turn;
                        }
                    }
                }
            }
        }

        for (size_t j{0}; j < current.size(); ++j)
        {
            for (size_t k{j + 1}; k < current.size(); ++k)
            {
                if (current.at(j) == current.at(k))
                {
                    if (scores.at(j) < scores.at(k))
                    {
                        fillPath(paths.at(k), maze);
                    }
                    else if (scores.at(j) > scores.at(k))
                    {
                        fillPath(paths.at(j), maze);
                    }

                    // handle situation where we now have a dead end
                    if (maze.countWalls(current.at(j).first, current.at(j).second) == 3)
                    {
                        // we'll use that there should be 3 paths
                        assert(current.size() == 3 && "Expected there to be 3 paths.\n");
                        size_t other {};
                        for (size_t l{0}; l < current.size(); ++l)
                        {
                            if (l != j && l != k)
                            {
                                other = l;
                                break;
                            }
                        }
                        // don't fill the last position
                        for (size_t m{0}; m < paths.at(other).size() - 1; ++m)
                        {
                            maze.fillMaze(paths.at(other).at(m).first, paths.at(other).at(m).second);
                        }
                    }
                }
            }
        }
    }

    template <std::size_t N>
    void removePointlessPaths(Maze<N>& maze)
    {
        size_t y {0};
        for (std::string& row : maze.getMaze())
        {
            for (size_t x{1}; x < row.size() - 1; ++x)
            {
                // interested in 3-way crossroads
                if (row[x] == '.' && maze.countWalls(x, y) == 1)
                {
                    fillIfPointlessPath<N>(x, y, maze);
                }
            }
            ++y;
        }
    }

    template <std::size_t N>
    InterVal getValues(Position pos, Move dir, const Maze<N>& maze)
    {
        Score score {0};
        do
        {
            if (isFree(goInDirection(pos, dir), maze) || reachExit(goInDirection(pos, dir), maze))
            {
                pos = goInDirection(pos, dir);
            }
            else
            {
                Moves nextMv {getOtherMoves(pos, dir, maze)};
                assert(nextMv.size() == 1 && "Expected only one option for next move.\n");
                dir = nextMv.at(0);
                pos = goInDirection(pos, dir);
                score += turn;
            }
            score += 1;
        } while (maze.countWalls(pos.first, pos.second) == 2);
        return {pos, dir, score};
    }

    template <std::size_t N>
    IntersectionMap getIntersectionDistances(const Maze<N>& maze)
    {
        IntersectionMap intersections {};
        Positions positions {maze.getStart()};

        while (positions.size() > 0)
        {
            Positions newPaths {};
            for (const Position& pos : positions)
            {
                for (Move dir : Direction::allDirections)
                {
                    if (intersections.find({pos, dir}) == intersections.end() && isFree(goInDirection(pos, dir), maze))
                    {
                        InterVal values {getValues(pos, dir, maze)};
                        intersections[{pos, dir}] = values;
                        InterKey key = std::make_pair(std::get<0>(values), std::make_pair(-std::get<1>(values).first, -std::get<1>(values).second));
                        intersections[key] = {pos, dir, std::get<2>(values)};

                        newPaths.push_back(std::get<0>(values));
                    }
                }
            }

            positions = newPaths;
        }

        return intersections;
    }

    void deleteDefunctPaths(Paths& paths, Moves& moves, Scores& scores, int index)
    {
        paths.erase(std::next(paths.begin(), index), std::next(paths.begin(), index + 1));
        moves.erase(std::next(moves.begin(), index), std::next(moves.begin(), index + 1));
        scores.erase(std::next(scores.begin(), index), std::next(scores.begin(), index + 1));
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
        Maze maze {Maze<N>(lines)};

        // remove dead ends, to make finding valid paths faster
        maze.removeDeadEnds();
        
        // go through maze and fill in pointless paths
        removePointlessPaths<N>(maze);

        IntersectionMap intersections {getIntersectionDistances<N>(maze)};

        Paths paths {};
        paths.push_back({maze.getStart()});
        // puzzle starts facing east
        Moves moves {Direction::east};
        Scores scores {0};

        Paths finalPaths {};
        Score minScore {};
        ShortestDistances shortest {};

        while (paths.size() > 0)
        {
            Paths pathsToAdd {};
            Moves movesToAdd {};
            Scores scoresToAdd {};
            Indexes pathsToEnd {};

            for (size_t i{0}; i < paths.size(); ++i)
            {
                Positions currPositions {paths.at(i)};
                Move currMove {moves.at(i)};
                Score currScore {scores.at(i)};

                for (Move dir : Direction::allDirections)
                {
                    InterKey key = std::make_pair(currPositions.back(), dir);

                    if (intersections.find(key) != intersections.end() && std::find(paths.at(i).begin(), paths.at(i).end(), std::get<0>(intersections.at(key))) == paths.at(i).end())
                    {
                        if (dir == currMove)
                        {
                            paths[i].push_back(std::get<0>(intersections.at(key)));
                            moves[i] = std::get<1>(intersections.at(key));
                            scores[i] = std::get<2>(intersections.at(key));

                            if (shortest.find(paths.at(i).back()) == shortest.end() || scores.at(i) < shortest.at(paths.at(i).back()))
                            {
                                shortest[paths.at(i).back()] = scores.at(i);
                            }

                            if (std::get<0>(intersections.at(key)) == maze.getEnd())
                            {
                                pathsToEnd.push_back(i);
                                if (minScore == 0 || scores.at(i) < minScore)
                                {
                                    minScore = scores.at(i);
                                    finalPaths = {paths.at(i)};
                                }
                                else if (scores.at(i) == minScore)
                                {
                                    finalPaths.push_back(paths.at(i));
                                }
                            }
                            else if ((minScore > 0 && scores.at(i) >= minScore) || scores.at(i) > shortest.at(paths.at(i).back()))
                            {
                                pathsToEnd.push_back(i);
                            }
                        }
                        else
                        {
                            Positions newPos {currPositions};
                            newPos.push_back(std::get<0>(intersections.at(key)));

                            if (std::get<0>(intersections.at(key)) == maze.getEnd())
                            {
                                Score finScore {currScore + std::get<2>(intersections.at(key)) + turn};

                                if (minScore == 0 || finScore < minScore)
                                {
                                    minScore = finScore;
                                    finalPaths = {newPos};
                                }
                                else if (finScore == minScore)
                                {
                                    finalPaths.push_back(newPos);
                                }
                            }
                            else
                            {
                                Score newScore {std::get<2>(intersections.at(key)) + turn};

                                if (shortest.find(paths.at(i).back()) == shortest.end() || newScore < shortest.at(paths.at(i).back()))
                                {
                                    shortest[paths.at(i).back()] = newScore;
                                }

                                if (minScore == 0 || newScore <= minScore || newScore <= shortest.at(paths.at(i).back()))
                                {
                                    pathsToAdd.push_back(newPos);
                                    movesToAdd.push_back(std::get<1>(intersections.at(key)));
                                    scoresToAdd.push_back(newScore);
                                }
                            }
                        }
                    }
                }
            }

            // add new paths/moves/scores
            for (size_t nw{0}; nw < pathsToAdd.size(); ++nw)
            {
                paths.push_back(pathsToAdd.at(nw));
                moves.push_back(movesToAdd.at(nw));
                scores.push_back(scoresToAdd.at(nw));
            }
            // remove defunct paths/moves/scores
            for (int ol{static_cast<int>(pathsToEnd.size() - 1)}; ol >= 0; --ol)
            {
                deleteDefunctPaths(paths, moves, scores, pathsToEnd[ol]);
            }
            std::cout << paths.size() << '\n';
        }

        return getTilesOnBestPaths(finalPaths);
    }
}

#endif // AOC16B_H
