#ifndef AOC16A_H
#define AOC16A_H

#include <string_view>
#include <array>
#include "../../shared/Direction.h"
#include <vector>
#include <set>
#include <cassert>
#include <algorithm>

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
    bool isFree(Position pos, const std::array<std::string_view, N> map)
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

    bool isPositionAlreadyReached(Position pos, const UniquePositions& uniquePositions)
    {
        return uniquePositions.count(pos) > 0;
    }

    template <std::size_t N>
    bool hasOnlyOnePath(Position pos, const std::array<std::string_view, N>& lines)
    {
        int count {0};
        for (Move move : Direction::allDirections)
        {
            if (isFree({pos.first + move.first, pos.second + move.second}, lines))
                ++count;
        }
        return count <= 2;
    }

    bool isLowScore(const Scores& scores, int score)
    {
        // add tolerance to account for some possible situations
        // int tolerance {5'500};
        for (size_t i{0}; i < scores.size(); ++i)
        {
            // no idea what this nonValidPaths bit was for
            // if (scores[i] + tolerance < score && std::find(nonValidPaths.begin(), nonValidPaths.end(), i) != nonValidPaths.end())
            // if (scores[i] + tolerance < score)
            if (scores[i] < score)
                return false;
        }
        return true;
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
        // can improve efficiency by tracking the lowest final score
        // and dropping paths that are higher than this
        int lowestFinalScore {};
        bool noPathsLeft {false};
        int count {1};
        while (!noPathsLeft)
        {
            // std::cout << '\t' << positions.size() << '\n';
            std::vector<size_t> nonValidPaths {};
            UniquePositions treadOldGround {};
            assert((positions.size() == movements.size() && movements.size() == scores.size()) && "Info on the number of paths is inconsistent (position, move, score).\n");
            Positions positionsToAdd {};
            Moves movesToAdd {};
            Scores scoresToAdd {};
            // std::cout << "loop " << count << '\n';
            for (size_t i{0}; i < positions.size(); ++i)
            {
                // std::cout << "iter " << i << ' ' << positions.size() << ' ' << positions[i].first << ' ' << positions[i].second << '\n';
                Position currentPos {positions[i]};
                Move currentMove {movements[i]};
                int currentScore {scores[i]};
                // first traverse the current direction
                Position forward {goInDirection(positions[i], movements[i])};
                // if path has higher score than current lowest
                // then there's no point continuing. End early for more speed
                if (lowestFinalScore > 0 && scores[i] >= lowestFinalScore)
                {
                    nonValidPaths.push_back(i);
                    continue;
                }
                // check that position wasn't reached already
                // i.e. avoid loops, or paths that are longer than needed
                // UNLESS path has the lowest score
                if (isPositionAlreadyReached(forward, uniquePos) && !reachExit(forward, lines) && !isLowScore(scores, scores[i]))
                    nonValidPaths.push_back(i);
                // otherwise path is valid
                else if (isFree(forward, lines))
                {
                    // std::cout << '\t' << i << '\n';
                    positions[i] = forward;
                    ++scores[i];
                    if (hasOnlyOnePath(forward, lines))
                        treadOldGround.insert(forward);
                }
                // if new position is exit, then path is done
                else if (reachExit(forward, lines))
                {
                    finalScores.push_back(scores[i] + 1);
                    if (lowestFinalScore == 0 || scores[i] + 1 < lowestFinalScore)
                        lowestFinalScore = scores[i] + 1;
                    std::cout << "a/ got " << scores[i] + 1 << " on " << count << '\n';
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
                    if (isPositionAlreadyReached(nextSpace, uniquePos) && !reachExit(nextSpace, lines) && !isLowScore(scores, currentScore))
                        continue;
                    // if path valid, then add as a new path
                    else if (isFree(nextSpace, lines))
                    {
                        // need to add these to temporary objects and add later
                        // as otherwise we could add extra loops in the for loop
                        positionsToAdd.push_back(nextSpace);
                        movesToAdd.push_back(otherMove);
                        // add 1,000 for turn and 1 for move
                        scoresToAdd.push_back(currentScore + 1001);
                        if (hasOnlyOnePath(nextSpace, lines))
                            treadOldGround.insert(nextSpace);
                    }
                    // if at exit, add 1,000 for turn and 1 for move
                    else if (reachExit(nextSpace, lines))
                    {
                        finalScores.push_back(currentScore + 1001);
                        if (lowestFinalScore == 0 || currentScore + 1001 < lowestFinalScore)
                            lowestFinalScore = currentScore + 1001;
                        std::cout << "b/ got " << currentScore + 1001 << " on " << count << '\n';
                    }
                }
                // std::cout << "reach end\n";
            }

            for (Position pos : treadOldGround)
                uniquePos.insert(pos);
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
            
            if (count == 431)
            {
                for (size_t y{0}; y < lines.size(); ++y)
                {
                    for (size_t x{0}; x < lines[y].size(); ++x)
                    {
                        if (uniquePos.find({x, y}) != uniquePos.end())
                            std::cout << '@';
                        else
                            std::cout << lines[y][x];
                    }
                    std::cout << '\n';
                }

                for (int score : finalScores)
                {
                    std::cout << score << ' ';
                }
                std::cout << '\n';
            }
            ++count;

            if (positions.size() == 0)
                noPathsLeft = true;
        }

        return getLowestScore(finalScores);
    }
}

#endif // AOC16A_H
