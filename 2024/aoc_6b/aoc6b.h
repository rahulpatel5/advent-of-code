#ifndef AOC6B_H
#define AOC6B_H

#include <string_view>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <stdexcept>
#include <cassert>
#include <algorithm>

/*
obstacle can only have an effect if it's on the existing path
so restrict search to solution for part 1 (despite what I originally did)

instead of going tile by tile, just go in a line
collect which rows and columns have obstacles
would only change direction if hitting an obstacle
this also makes adding an obstacle easier
*/

namespace aoc6b
{
    using PosInt = int;
    using Position = std::pair<PosInt, PosInt>;
    using Positions = std::vector<Position>;
    using UniquePositions = std::set<Position>;
    using ObstacleMap = std::map<PosInt, std::set<PosInt>>;
    using Obstacles = std::vector<PosInt>;
    using Direction = std::pair<PosInt, PosInt>;
    using Directions = std::vector<Direction>;

    Direction getStartDirection(char c)
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

    template <std::size_t N>
    void processMapAndGetInputs(Position& startPos, Direction& startDir, ObstacleMap& obstaclesByRow, ObstacleMap& obstaclesByCol, const std::array<std::string_view, N>& lines)
    {
        for (PosInt row{0}; row < lines.size(); ++row)
        {
            for (PosInt col{0}; col < lines.at(row).size(); ++col)
            {
                // collect obstacles in form {x,y} ({col,row})
                if (lines.at(row).at(col) == '#')
                {
                    obstaclesByRow[row].insert(col);
                    obstaclesByCol[col].insert(row);
                }

                // get start position and direction in same {x,y} form
                else if (lines.at(row).at(col) != '.')
                {
                    startPos = {col, row};
                    startDir = getStartDirection(lines.at(row).at(col));
                }
            }
        }
    }

    Direction turnRight(Direction currentDirection)
    {
        if ((currentDirection.first == 0) && (currentDirection.second == -1))
            return { 1,  0 };
        else if ((currentDirection.first == 1) && (currentDirection.second == 0))
            return { 0,  1 };
        else if ((currentDirection.first == 0) && (currentDirection.second == 1))
            return {-1,  0 };
        else if ((currentDirection.first == -1) && (currentDirection.second == 0))
            return { 0, -1 };
        else
            throw std::invalid_argument("received invalid previous direction");
    }

    bool comparePositions(PosInt obstacle, PosInt axisPos, PosInt direction)
    {
        if (direction > 0)
            return obstacle > axisPos;
        else
            return obstacle < axisPos;
    }

    template <std::size_t N>
    Positions getOriginalPath(const std::array<std::string_view, N>& lines, const Position& startPos, const Direction& startDir, const ObstacleMap& obstaclesByRow, const ObstacleMap& obstaclesByCol, Directions& pathDirections)
    {
        Positions path {};
        path.push_back(startPos);
        
        Position position {startPos};
        Direction direction {startDir};
        pathDirections.push_back(direction);

        bool isBlockingObstacle {true};
        while (isBlockingObstacle)
        {
            isBlockingObstacle = false;

            // we only move one tile in one direction (N/E/S/W)
            // we move (-)1 in that direction and 0 in the other
            PosInt moveDir {direction.first + direction.second};
            PosInt changingPos {};
            PosInt fixedPos {};
            if (direction.first != 0)
            {
                changingPos = position.first;
                fixedPos = position.second;
            }
            else
            {
                changingPos = position.second;
                fixedPos = position.first;
            }
            
            // this is wasteful, but is more concise for now
            const ObstacleMap& obstacleInDir {(direction.first != 0) ? obstaclesByRow : obstaclesByCol};
            
            // set default value, which works for west and north
            PosInt limitPos {-1};
            // change this for south or east
            if (direction.second > 0)
                limitPos = lines.size();
            else if (direction.first > 0)
                limitPos = lines.at(position.second).size();
            PosInt defaultValue {limitPos};
            
            PosInt obstaclePos {std::move(limitPos)};
            Obstacles obstacles {obstacleInDir.at(fixedPos).begin(), obstacleInDir.at(fixedPos).end()};
            if (moveDir < 0)
                std::reverse(obstacles.begin(), obstacles.end());
            for (PosInt obstacle : obstacles)
            {
                if (comparePositions(obstacle, changingPos, moveDir))
                {
                    obstaclePos = obstacle;
                    break;
                }
            }
            // add the tiles to the path
            for (PosInt i{changingPos + moveDir}; i != obstaclePos; i += moveDir)
            {
                if (direction.first != 0)
                    path.push_back({i, fixedPos});
                else
                    path.push_back({fixedPos, i});
                pathDirections.push_back(direction);
            }
            // set the new position and direction
            if (direction.first != 0)
                position.first = obstaclePos - moveDir;
            else
                position.second = obstaclePos - moveDir;
            direction = turnRight(direction);
            // set whether we've exited the map
            if (obstaclePos != defaultValue)
                isBlockingObstacle = true;
        }

        return path;
    }

    template <std::size_t N>
    bool isLoop(const std::array<std::string_view, N>& lines, const Position& startPos, const Direction& startDir, const ObstacleMap& obstaclesByRow, const ObstacleMap& obstaclesByCol, const Position& newObstacle)
    {
        Position position {startPos};
        Direction direction {startDir};

        UniquePositions uniquePos {};
        int dupeCount {0};
        
        bool isBlockingObstacle {true};
        while (isBlockingObstacle)
        {
            isBlockingObstacle = false;

            // we only move one tile in one direction (N/E/S/W)
            // we move (-)1 in that direction and 0 in the other
            PosInt moveDir {direction.first + direction.second};
            PosInt changingPos {};
            PosInt fixedPos {};
            if (direction.first != 0)
            {
                changingPos = position.first;
                fixedPos = position.second;
            }
            else
            {
                changingPos = position.second;
                fixedPos = position.first;
            }
            
            // this is wasteful, but is more concise for now
            const ObstacleMap& obstacleInDir {(direction.first != 0) ? obstaclesByRow : obstaclesByCol};
            
            // set default value, which works for west and north
            PosInt limitPos {-1};
            // change this for south or east
            if (direction.second > 0)
                limitPos = lines.size();
            else if (direction.first > 0)
                limitPos = lines.at(position.second).size();
            PosInt defaultValue {limitPos};
            
            PosInt obstaclePos {std::move(limitPos)};
            Obstacles obstacles {};
            if (obstacleInDir.find(fixedPos) != obstacleInDir.end())
                obstacles.insert(obstacles.end(), obstacleInDir.at(fixedPos).begin(), obstacleInDir.at(fixedPos).end());

            // we need to add the new obstacle, if it's on this col/row
            // don't bother with this if new obstacle is in opp direction
            if (direction.first != 0 && newObstacle.second == fixedPos && ((newObstacle.first < position.first && direction.first < 0) || (newObstacle.first > position.first && direction.first > 0)))
            {
                obstacles.push_back(newObstacle.first);
                if (obstacles.size() > 1)
                    std::sort(obstacles.begin(), obstacles.end());
            }
            else if (direction.second != 0 && newObstacle.first == fixedPos && ((newObstacle.second < position.second && direction.second < 0) || (newObstacle.second > position.second && direction.second > 0)))
            {
                obstacles.push_back(newObstacle.second);
                if (obstacles.size() > 1)
                    std::sort(obstacles.begin(), obstacles.end());
            }
            
            // if there are no obstacles in this row/col, then leaving map
            if (obstacles.size() == 0)
                break;
            if (moveDir < 0)
                std::reverse(obstacles.begin(), obstacles.end());
            for (PosInt obstacle : obstacles)
            {
                if (comparePositions(obstacle, changingPos, moveDir))
                {
                    obstaclePos = obstacle;
                    break;
                }
            }

            // set the new position and direction
            if (direction.first != 0)
                position.first = obstaclePos - moveDir;
            else
                position.second = obstaclePos - moveDir;
            direction = turnRight(direction);

            // if this position is in uniquePos, we've got a loop
            if (uniquePos.find(position) != uniquePos.end())
            {
                ++dupeCount;
                if (dupeCount > 1)
                    return true;
            }
            else
                dupeCount = 0;
            // otherwise we'll continue, by adding the position to uniquePos
            uniquePos.insert(position);

            // set whether we've exited the map
            if (obstaclePos != defaultValue)
                isBlockingObstacle = true;
        }
        // we must have left the map, so there's no loop
        return false;
    }

    template <std::size_t N>
    PosInt addObstaclesAndCountLoops(const Positions& originalPath, const Directions& pathDirections, const std::array<std::string_view, N>& lines, const Position& startPos, const Direction& startDir, const ObstacleMap& obstaclesByRow, const ObstacleMap& obstaclesByCol)
    {
        UniquePositions triedObstacles {};
        PosInt loops {0};
        // skip first position since it's not a loop and can't add obstacle
        triedObstacles.insert(originalPath.at(0));
        for (size_t i{1}; i < originalPath.size(); ++i)
        {
            if (triedObstacles.find(originalPath.at(i)) != triedObstacles.end() || originalPath.at(i) == startPos)
                continue;
            
            if (isLoop<N>(lines, startPos, startDir, obstaclesByRow, obstaclesByCol, originalPath.at(i)))
                ++loops;
            
            triedObstacles.insert(originalPath.at(i));
        }
        return loops;
    }

    template <std::size_t N, std::size_t width>
    PosInt parseAndCountDistinctLoops(const std::array<std::string_view, N>& lines)
    {
        // more efficient if we iterate through the map once
        // and collect all the variables we want at once
        Position startPos {};
        Direction startDir {};
        ObstacleMap obstaclesByRow {};
        ObstacleMap obstaclesByCol {};
        processMapAndGetInputs<N>(startPos, startDir, obstaclesByRow, obstaclesByCol, lines);
        assert(obstaclesByRow.size() == obstaclesByCol.size() && "Error: expected obstacle maps to be same size.\n");

        Directions pathDirections {};
        Positions originalPath {getOriginalPath<N>(lines, startPos, startDir, obstaclesByRow, obstaclesByCol, pathDirections)};

        return addObstaclesAndCountLoops<N>(originalPath, pathDirections, lines, startPos, startDir, obstaclesByRow, obstaclesByCol);
    }
}

#endif // AOC6B_H
